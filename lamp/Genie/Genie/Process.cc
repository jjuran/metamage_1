/*	===============
 *	GenieProcess.cc
 *	===============
 */

#include "Genie/Process.hh"

// Standard C/C++
//#include <csignal>
#include <signal.h>

// POSIX
#include "sys/stat.h"
#include "unistd.h"

// Nucleus
#include "Nucleus/NAssert.h"

// Nitrogen / Carbon support
#include "Nitrogen/Aliases.h"
#include "Nitrogen/MacErrors.h"
#include "Nitrogen/OSStatus.h"
#include "Nitrogen/Sound.h"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"
#include "Utilities/Threads.h"

// POSeven
#include "POSeven/Errno.hh"

// Io
#include "Io/Stream.hh"

// Kerosene/Common
#include "KEnvironment.hh"

// Genie
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/Yield.hh"


#if defined(__MWERKS__) && defined(__csignal__)
	#undef SIGABRT
	#undef SIGFPE
	#undef SIGILL
	#undef SIGINT
	#undef SIGSEGV
	#undef SIGTERM
#endif

#if 0
#define	SIGHUP	1	/* hangup */
#define	SIGINT	2	/* interrupt */
#define	SIGQUIT	3	/* quit */
#define	SIGILL	4	/* illegal instruction (not reset when caught) */
#define	SIGTRAP	5	/* trace trap (not reset when caught) */
#define	SIGABRT	6	/* abort() */
#define	SIGIOT	SIGABRT	/* compatibility */
#define	SIGEMT	7	/* EMT instruction */
#define	SIGFPE	8	/* floating point exception */
#define	SIGKILL	9	/* kill (cannot be caught or ignored) */
#define	SIGBUS	10	/* bus error */
#define	SIGSEGV	11	/* segmentation violation */
#define	SIGSYS	12	/* bad argument to system call */
#define	SIGPIPE	13	/* write on a pipe with no one to read it */
#define	SIGALRM	14	/* alarm clock */
#define	SIGTERM	15	/* software termination signal from kill */
#define	SIGURG	16	/* urgent condition on IO channel */
#define	SIGSTOP	17	/* sendable stop signal not from tty */
#define	SIGTSTP	18	/* stop signal from tty */
#define	SIGCONT	19	/* continue a stopped process */
#define	SIGCHLD	20	/* to parent on child stop or exit */
#define	SIGTTIN	21	/* to readers pgrp upon background tty read */
#define	SIGTTOU	22	/* like TTIN for output if (tp->t_local&LTOSTOP) */
#define	SIGIO	23	/* input/output possible signal */
#define	SIGXCPU	24	/* exceeded CPU time limit */
#define	SIGXFSZ	25	/* exceeded file size limit */
#define	SIGVTALRM 26	/* virtual time alarm */
#define	SIGPROF	27	/* profiling time alarm */
#define SIGWINCH 28	/* window size changes */
#define SIGINFO	29	/* information request */
#define SIGUSR1 30	/* user defined signal 1 */
#define SIGUSR2 31	/* user defined signal 2 */
#endif


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace NX = NitrogenExtras;
	namespace P7 = POSeven;
	namespace Sh = ShellShock;
	namespace K = Kerosene;
	
	
	GenieProcessTable gProcessTable;
	
	int ExternalProcessExecutor::operator()( ThreadContext& context ) const
	{
		context.processContext->InitThread();
		
		RegisterProcessContext( context.processContext );
		
		int result = -1;
		
		if ( context.externalMain != NULL )
		{
			result = context.externalMain( Sh::CountStringArray( context.argv ),
			                               context.argv,
			                               context.envp );
		}
		
		context.processContext->Terminate( (result & 0xFF) << 8 );
		
		// This is a necessary hack for now to keep the thread from terminating naturally.
		// If that happens, the ThreadID Disposer throws -618 (thread not found)
		//context.processContext->KillThread();
		
		return result;
	}
	
	struct ExecContext
	{
		FSSpec                      executable;
		std::vector< const char* >  argVector;
		std::string                 scriptPath;
		std::string                 interpreterPath;
		std::string                 interpreterArg;
		
		ExecContext()  {}
		
		ExecContext( const FSSpec&       executable,
		             char const* const*  argv )
		:
			executable( executable ),
			argVector ( argv, argv + Sh::CountStringArray( argv ) + 1 )
		{}
	};
	
	inline P7::Errno NotExecutable()  { return P7::Errno( EPERM ); }
	
	static ExecContext& Normalize( ExecContext& context, const FSTreePtr& cwd )
	{
		const OSType type = N::FSpGetFInfo( context.executable ).fdType;
		
		if ( type == 'Wish' )
		{
			return context;  // Already normalized
		}
		
		if ( type == 'TEXT' )
		{
			context.interpreterPath = "/bin/sh";  // default
			bool hasArg = false;
			
			using N::fsRdPerm;
			
			NN::Owned< N::FSFileRefNum > script = N::FSpOpenDF( context.executable, fsRdPerm );
			
			char data[ 1024 + 1 ];
			data[1024] = '\0';
			
			size_t bytes = N::FSRead( script, 1024, data );
			
			N::FSClose( script );
			
			if ( bytes > 2 && data[0] == '#' && data[1] == '!' )
			{
				char* cr = std::strchr( data, '\r' );
				char* lf = std::strchr( data, '\n' );
				
				char* nl = (cr && lf) ? std::min( cr, lf ) : (cr ? cr : lf);
				
				if ( nl == NULL )
				{
					throw NotExecutable();  // #! line too long
				}
				
				*nl = '\0';
				
				char* space = std::strchr( data, ' ' );
				
				hasArg = space;
				
				context.interpreterPath = std::string( &data[2], space ? space : nl );
				context.interpreterArg  = std::string( space ? space + 1 : nl, nl );
			}
			
			const int newTokenCount = 2 + hasArg;
			
			// E.g. "$ script foo bar baz"
			// argv == { "script", "foo", "bar", "baz", NULL }
			
			context.argVector.resize( context.argVector.size() + newTokenCount );
			
			const char* const* const argv = &context.argVector.front();
			
			// argv == { "script", "foo", "bar", "baz", NULL, ??, ?? }
			
			bool pathSearched = std::strchr( argv[0], '/' ) == NULL;
			
			const int skipCount = pathSearched;  // skip the script's name if we're overwriting it anyway
						
			std::copy_backward( context.argVector.begin() + skipCount,
			                    context.argVector.end() - newTokenCount,
			                    context.argVector.end() );
			
			// argv == { "script", "foo", "script", "foo", "bar", "baz", NULL }
			
			context.argVector[ 0 ] = context.interpreterPath.c_str();
			context.argVector[ 1 ] = context.interpreterArg.c_str();
			context.argVector[ 1 + hasArg ] = "--";
			
			if ( pathSearched )
			{
				// Overwrite with full pathname
				context.scriptPath = N::FSpGetPOSIXPathname( context.executable );
				
				context.argVector[ 2 + hasArg ] = context.scriptPath.c_str();
			}
			
			// argv == { "sh", "--", "script", "foo", "bar", "baz", NULL }
			
			context.executable = ResolvePathname( context.interpreterPath, cwd )->GetFSSpec();
		}
		else if ( type == 'MPST' )
		{
			context.scriptPath = N::FSpGetMacPathname( context.executable );
			
			const int newTokenCount = 3;
			const int skipCount = 1;  // skip the script's name because we're overwriting it anyway
			
			// E.g. "$ script foo bar"
			// argv == { "script", "foo", "bar", "baz", NULL }
			
			context.argVector.resize( context.argVector.size() + newTokenCount );
			
			const char* const* const argv = &context.argVector.front();
			
			// argv == { "script", "foo", "bar", "baz", NULL, ??, ?? }
			
			std::copy_backward( context.argVector.begin() + skipCount,
			                    context.argVector.end() - newTokenCount,
			                    context.argVector.end() );
			
			// argv == { "script", "foo", "bar", "foo", "bar", "baz", NULL }
			
			context.argVector[ 0 ] = "/usr/bin/tlsrvr";
			context.argVector[ 1 ] = "--escape";
			context.argVector[ 2 ] = "--";
			context.argVector[ 3 ] = context.scriptPath.c_str();  // Overwrite with full pathname
			
			// argv == { "sh", "--", "/usr/bin/script", "foo", "bar", "baz", NULL }
			
			context.executable = ResolvePathname( "/usr/bin/tlsrvr", cwd )->GetFSSpec();
		}
		else
		{
			throw NotExecutable();
		}
		
		return context;
	}
	
	Process::Process( RootProcess ) 
	:
		itsPPID               ( 0 ),
		itsPID                ( gProcessTable.NewProcess( this ) ),
		itsPGID               ( 0 ),
		itsSID                ( 0 ),
		itsPendingSignals     ( 0 ),
		itsPreviousSignals    ( 0 ),
		itsName               ( "init" ),
		itsCWD                ( FSRoot() ),
		itsControllingTerminal( NULL ),
		itsFileDescriptors    ( FileDescriptorMap() ),
		itsStatus             ( kStarting ),
		itsEnvironStorage     ( new Sh::VarArray() ),
		itsErrnoData          ( NULL ),
		itsEnvironData        ( NULL )
	{
	}
	
	Process::Process( pid_t ppid ) 
	:
		itsPPID               ( ppid ),
		itsPID                ( gProcessTable.NewProcess( this ) ),
		itsPGID               ( gProcessTable[ ppid ].GetPGID() ),
		itsSID                ( gProcessTable[ ppid ].GetSID() ),
		itsPendingSignals     ( 0 ),
		itsPreviousSignals    ( 0 ),
		itsName               ( gProcessTable[ ppid ].ProgramName() ),
		itsCWD                ( gProcessTable[ ppid ].CurrentWorkingDirectory() ),
		itsControllingTerminal( gProcessTable[ ppid ].itsControllingTerminal ),
		itsFileDescriptors    ( gProcessTable[ ppid ].FileDescriptors() ),
		itsStatus             ( kStarting ),
		itsEnvironStorage     ( new Sh::VarArray( gProcessTable[ ppid ].itsEnvironStorage->GetPointer() ) ),
		itsErrnoData          ( gProcessTable[ ppid ].itsErrnoData ),
		itsEnvironData        ( gProcessTable[ ppid ].itsEnvironData )
	{
		// The environment data and the pointer to the fragment's environ
		// variable have been copied from the parent.
		// The calling fragment is now associated with the new process.
		// Update its environ to point to our storage.
		if ( itsEnvironData != NULL )
		{
			*itsEnvironData = itsEnvironStorage->GetPointer();
		}
	}
	
	void Process::InitThread()
	{
		N::CloseConnection( itsOldFragmentConnection );
		
		itsOldFragmentImage = BinaryImage();
	}
	
	void Process::KillThread()
	{
		// Kill the thread.
		
		// This is bad if we're killing ourselves, since the very thread we're killing
		// is the one performing the operation, which therefore won't complete.
		itsThread.reset( NULL );
	}
	
	template < class Type >
	bool LoadSymbol( N::CFragConnectionID fragment, ConstStr255Param symName, const Type& init )
	{
		
		try
		{
			Type* symbol = NULL;
			
			N::FindSymbol( fragment, symName, &symbol );
			
			*symbol = init;
		}
		catch ( ... )
		{
			return false;
		}
		
		return true;
	}
	
	class SymbolImporter
	{
		private:
			N::CFragConnectionID itsFragmentConnection;
		
		public:
			SymbolImporter( N::CFragConnectionID conn ) : itsFragmentConnection( conn )  {}
			
			bool operator()( SystemCallRegistry::value_type systemCall ) const
			{
				std::string name = systemCall.first;
				void*       func = systemCall.second;
				
				name += "_import_";
				
				return LoadSymbol( itsFragmentConnection, N::Str255( name ), func );
			}
	};
	
	static void ImportSystemCalls( N::CFragConnectionID fragmentConnection )
	{
		std::for_each( SystemCallsBegin(),
		               SystemCallsEnd(),
		               SymbolImporter( fragmentConnection ) );
	}
	
	int Process::Exec( const FSSpec&        executable,
	                   const char* const    argv[],
	                   const char* const*   envp )
	{
		// Close file descriptors with close-on-exec flag.
		typedef FileDescriptorMap::iterator FDIter;
		
		std::vector< int > toClose;
		
		for ( FDIter it = itsFileDescriptors.begin();  it != itsFileDescriptors.end();  ++it )
		{
			if ( it->second.closeOnExec )
			{
				toClose.push_back( it->first );
			}
		}
		
		typedef std::vector< int >::const_iterator ToCloseIter;
		
		for ( ToCloseIter it = toClose.begin();  it != toClose.end();  ++it )
		{
			itsFileDescriptors.erase( *it );
		}
		
		// Do we take the name before or after normalization?
		ProgramName( NN::Convert< std::string >( executable.name ) );
		
		ExecContext context( executable, argv );
		
		Normalize( context, CurrentWorkingDirectory() );
		
		FSSpec programFile = context.executable;
		
		// Save the fragment image and the connection that we're running from.
		// We can't use stack storage because we run the risk of the thread terminating.
		itsOldFragmentImage      = itsFragmentImage;
		itsOldFragmentConnection = itsFragmentConnection;
		
		{
			BinaryImage binary = GetBinaryImage( programFile );
		}
		
		//itsFragmentImage = ReadFragmentImageFromPluginFile( programFile );
		itsFragmentImage = GetBinaryImage( programFile );
		
		itsFragmentConnection = N::GetMemFragment< kPrivateCFragCopy >( itsFragmentImage.GetPointer(),
		                                                                itsFragmentImage.GetSize() );
		
		K::Versions assumedVersions;
		
		assumedVersions.current        = 2;
		assumedVersions.lastCompatible = 2;
		
		K::Versions* versions = &assumedVersions;
		
		MainProcPtr mainEntryPoint = NULL;
		
		try
		{
			N::FindSymbol( itsFragmentConnection, "\p" "main", &mainEntryPoint );
		}
		catch ( ... )
		{
			//Io::Stream< IORef > Err = FileDescriptors()[ 2 ].handle;
			
			//Err << "Missing main() entry point.\n";
			throw;
		}
		
		try
		{
			N::FindSymbol( itsFragmentConnection, "\p" "gVersions", &versions );
		}
		catch ( ... )
		{
			// The practice of exporting main() postdates that of exporting gVersions.
			// Therefore, we can conclude that a drop-in which exports main() but omits
			// gVersions does so deliberately, whereas one which omits both we must
			// assume to be indefinitely old.
			// The latter case currently results in a warning, and subsequently
			// (once the protocol incompatibly changes) an error.
			// But the former indicates a program with zero dependence on the
			// kernel interface, and therefore ought not be version-checked.
			// (If such a program supplied version information, then a future version
			// of Genie with kLastCompatibleLibVersion augmented would needlessly
			// refuse to run it.)
			if ( mainEntryPoint == NULL )
			{
				//Io::Stream< IORef > Err = FileDescriptors()[ 2 ].handle;
				
				//Err << "No version data found.\n";
			}
		}
		
		using K::kCurrentVersion;
		using K::kLastCompatibleLibVersion;
		
		if ( kCurrentVersion > versions->current )
		{
			/*
			Io::Stream< IORef > Err = FileDescriptors()[ 2 ].handle;
			
			// Plugin interface is older than what we know
			Err << "Plugin uses interface version "
			    << versions->current
			    << "\n";
			*/
			
			if ( versions->current < kLastCompatibleLibVersion )
			{
				// It's too old
				//Err << "The plugin is too old\n";
				throw N::CFragImportTooOldErr();
			}
		}
		else if ( kCurrentVersion < versions->current )
		{
			/*
			Io::Stream< IORef > Err = FileDescriptors()[ 2 ].handle;
			
			// Plugin interface is newer than we know
			Err << "Plugin uses interface version "
			    << versions->current
			    << "\n";
			*/
			
			if ( kCurrentVersion < versions->lastCompatible )
			{
				// We're too old
				//Err << "The plugin is too new\n";
				throw N::CFragImportTooNewErr();
			}
		}
		
		itsArgvStorage.reset( new Sh::StringArray( &context.argVector[ 0 ] ) );
		
		// We need to set the calling fragment's environ back to the parent
		// process' environment storage.
		if ( itsEnvironData != NULL )
		{
			*itsEnvironData = itsPPID > 0 ? gProcessTable[ itsPPID ].itsEnvironStorage->GetPointer()
			                              : NULL;
		}
		
		// Reset the environment storage as a copy of envp (or empty).
		// As an optimization, if the pointers match, then envp must be the
		// current environ, and the data are already there.
		// In that case, we just update the new fragment's environ to point to us.
		
		if ( envp == NULL )
		{
			itsEnvironStorage.reset( new Sh::VarArray );
		}
		else if ( envp != itsEnvironStorage->GetPointer() )
		{
			itsEnvironStorage.reset( new Sh::VarArray( envp ) );
		}
		
		try
		{
			itsEnvironData = NULL;
			
			N::FindSymbol( itsFragmentConnection, "\p" "environ", &itsEnvironData );
			
			*itsEnvironData = itsEnvironStorage->GetPointer();
		}
		catch ( ... ) {}
		
		try
		{
			itsErrnoData = NULL;
			
			N::FindSymbol( itsFragmentConnection, "\p" "errno", &itsErrnoData );
		}
		catch ( ... ) {}
		
		ImportSystemCalls( itsFragmentConnection );
		
		Result( 0 );
		
		ThreadContext threadContext( this,
		                             mainEntryPoint,
		                             itsArgvStorage->GetPointer(),
		                             itsEnvironStorage->GetPointer() );
		
		// Warning -- if a program calls exec() without forking,
		// then the current thread might be the one we're about to replace.
		// Make sure the assignment is complete before disposing the thread.
		
		std::auto_ptr< Thread > savedThread = itsThread;
		itsThread.reset( new Thread( threadContext ) );
		
		Status( Process::kRunning );
		
		savedThread.reset( NULL );  // If this is us, we're toast
		
		// If we're still here, then we must have been forked.
		
		return 0;
	}
	
	/*
	int Process::TryExec( const FSSpec&        executable,
	                      const char* const    argv[],
	                      const char* const*   envp )
	{
		try
		{
			return Exec( executable, argv, envp );
		}
		catch ( N::OSStatusErrMessage< N::CFragNoLibraryErr >& err )
		{
			Io::S( FileDescriptors()[ 2 ].handle ) << "cFragNoLibraryErr: "
			                                       << err.errMessage
			                                       << "\n";
			throw;
		}
		catch ( N::OSStatusErrMessage< N::CFragUnresolvedErr >& err )
		{
			Io::S( FileDescriptors()[ 2 ].handle ) << "cFragUnresolvedErr: "
			                                       << err.errMessage
			                                       << "\n";
			throw;
		}
		catch ( N::ErrMessage& err )
		{
			try
			{
				throw;
			}
			catch ( N::OSStatus status )
			{
				Io::S( FileDescriptors()[ 2 ].handle ) << "Caught OSStatus "
				                                       << status
				                                       << " with errMessage "
				                                       << err.errMessage
				                                       << ".\n";
			}
			catch ( ... )
			{
				Io::S( FileDescriptors()[ 2 ].handle ) << "Caught exception with errMessage "
				                                       << err.errMessage
				                                       << " which isn't an OSStatus.  This shouldn't happen.\n";
			}
			
			throw;
		}
		catch ( N::OSStatus err )
		{
			Io::S( FileDescriptors()[ 2 ].handle ) << "Couldn't load plugin:  " << err << "\n";
			throw;
		}
		
		// Not reached...
		return -1;
	}
	*/
	
	int Process::SetErrno( int errorNumber )
	{
		if ( itsErrnoData != NULL )
		{
			*itsErrnoData = errorNumber;
		}
		
		return errorNumber == 0 ? 0 : -1;
	}
	
	char* Process::GetEnv( const char* name )
	{
		char* result = NULL;
		
		Sh::SVector::const_iterator it = itsEnvironStorage->Find( name );
		
		const char* var = *it;
		
		const char* end = Sh::EndOfVarName( var );
		
		// Did we find the right environment variable?
		if ( end != NULL  &&  *end == '='  &&  Sh::VarMatchesName( var, end, name ) )
		{
			itsLastEnv = var;
			itsLastEnv += "\0";  // make sure we have a trailing null
			
			std::size_t offset = end + 1 - var;
			
			result = &itsLastEnv[ offset ];
		}
		
		return result;
	}
	
	void Process::SetEnv( const char* name, const char* value, bool overwrite )
	{
		Sh::SVector::iterator it = itsEnvironStorage->Find( name );
		
		const char* var = *it;
		
		// Did we find the right environment variable?
		bool match = Sh::VarMatchesName( var, Sh::EndOfVarName( var ), name );
		
		// If it doesn't match, we insert (otherwise, we possibly overwrite)
		bool inserting = !match;
		
		if ( inserting )
		{
			itsEnvironStorage->Insert( it, Sh::MakeVar( name, value ) );
			
			if ( itsEnvironData )
			{
				*itsEnvironData = itsEnvironStorage->GetPointer();
			}
		}
		else if ( overwrite )
		{
			itsEnvironStorage->Overwrite( it, Sh::MakeVar( name, value ) );
		}
	}
	
	void Process::PutEnv( const char* string )
	{
		std::string name = string;
		name.resize( name.find( '=' ) );
		
		Sh::SVector::iterator it = itsEnvironStorage->Find( name.c_str() );
		
		const char* var = *it;
		
		// Did we find the right environment variable?
		bool match = Sh::VarMatchesName( var, Sh::EndOfVarName( var ), name.c_str() );
		
		// If it doesn't match, we insert (otherwise, we possibly overwrite)
		bool inserting = !match;
		
		if ( inserting )
		{
			itsEnvironStorage->Insert( it, string );
			
			if ( itsEnvironData )
			{
				*itsEnvironData = itsEnvironStorage->GetPointer();
			}
		}
		else
		{
			itsEnvironStorage->Overwrite( it, string );
		}
	}
	
	void Process::UnsetEnv( const char* name )
	{
		Sh::SVector::iterator it = itsEnvironStorage->Find( name );
		
		const char* var = *it;
		
		// Did we find the right environment variable?
		bool match = Sh::VarMatchesName( var, Sh::EndOfVarName( var ), name );
		
		if ( match )
		{
			itsEnvironStorage->Remove( it );
		}
	}
	
	void Process::ClearEnv()
	{
		itsEnvironStorage->Clear();
		
		if ( itsEnvironData )
		{
			*itsEnvironData = NULL;
		}
	}
	
	/*
	long Process::ChangeDirectory( const N::FSDirSpec& dir )
	{
		myCWD = dir;
		return 0;
	}
	*/
	
	int Process::ChangeDirectory( const char* pathname )
	{
		if ( pathname != NULL )
		{
			FSTreePtr dir = ResolvePathname( pathname, CurrentWorkingDirectory() );
			
			struct ::stat sb;
			
			dir->Stat( sb );  // Throws if nonexistent
			
			if ( !(sb.st_mode & S_IFDIR) )
			{
				P7::ThrowErrno( ENOTDIR );
			}
			
			itsCWD = dir;
		}
		else
		{
			// What constitutes home here?
		}
		
		return 0;
	}
	
	void Process::Terminate()
	{
		Status( kTerminated );
		
		itsFileDescriptors.clear();
		
		int ppid = ParentProcessID();
		int pid = ProcessID();
		
		if ( ppid > 0 )
		{
			gProcessTable[ ppid ].Raise( SIGCHLD );
		}
		
		typedef GenieProcessTable::ProcessMap::const_iterator const_iterator;
		
		for ( const_iterator it = gProcessTable.begin();  it != gProcessTable.end();  ++it )
		{
			Process& proc = *( *it ).second;
			
			if ( proc.ParentProcessID() == pid )
			{
				proc.Raise( SIGHUP );
				proc.Orphan();
			}
		}
	}
	
	void Process::Terminate( int result )
	{
		itsResult = result;
		
		Terminate();
	}
	
	void Process::Orphan()
	{
		ASSERT( itsPID != 1 );
		
		itsPPID = 1;
		itsFileDescriptors.clear();
	}
	
	sig_t Process::SetSignalAction( int signal, sig_t signalAction )
	{
		if ( signal == SIGKILL  ||  signal == SIGSTOP  ||  signalAction == SIG_ERR )
		{
			//errno = EINVAL;  // FIXME
			return SIG_ERR;
		}
		
		sig_t result = itsSignalMap[ signal ];
		
		itsSignalMap[ signal ] = signalAction;
		
		return result;
	}
	
	void Process::Raise( int signal )
	{
		sig_t action = itsSignalMap[ signal ];
		
		if ( action == SIG_IGN )
		{
			return;
		}
		
		if ( action == SIG_DFL )
		{
			if ( ProcessID() == 1 )
			{
				return;
			}
			
			switch ( signal )
			{
				case SIGQUIT:
				case SIGILL:
				case SIGTRAP:
				case SIGABRT:
				//case SIGEMT:
				case SIGFPE:
				case SIGBUS:
				case SIGSEGV:
				case SIGSYS:
					// create core image
					// for now, fall through
					//break;
				case SIGHUP:
				case SIGINT:
				case SIGKILL:
				case SIGPIPE:
				case SIGALRM:
				case SIGTERM:
				case SIGXCPU:
				case SIGXFSZ:
				case SIGVTALRM:
				case SIGPROF:
				case SIGUSR1:
				case SIGUSR2:
					// terminate process
					Terminate( signal );
					KillThread();
					break;
				case SIGCONT:
				case SIGURG:
				case SIGCHLD:
				case SIGIO:
				case SIGWINCH:
				//case SIGINFO:
					// continue and discard signal
					Continue();
					break;
				case SIGSTOP:
				case SIGTSTP:
				case SIGTTIN:
				case SIGTTOU:
					// stop process
					Stop();
					break;
				default:
					// bad signal
					break;
			}
		}
		else
		{
			// FIXME:  Block this signal during the function call
			itsPendingSignals |= 1 << signal - 1;
			
			Continue();
		}
	}
	
	bool Process::HandlePendingSignals()
	{
		itsPreviousSignals = itsPendingSignals;
		
		UInt32 bits = itsPendingSignals;
		int signal = 1;
		
		while ( bits )
		{
			if ( bits & 1 )
			{
				sig_t action = itsSignalMap[ signal ];
				
				ASSERT( action != SIG_IGN );
				ASSERT( action != SIG_DFL );
				
				action( signal );
				
				itsPendingSignals &= ~( 1 << signal - 1 );
			}
			
			bits >>= 1;
			++signal;
		}
		
		return itsPreviousSignals;
	}
	
	GenieProcessTable::GenieProcessTable() : itsNextPID( 1 )
	{
		new Process( Process::RootProcess() );
	}
	
	Process& GenieProcessTable::operator[]( pid_t pid )
	{
		ProcessMap::iterator it = itsProcesses.find( pid );
		
		if ( it == itsProcesses.end() )
		{
			throw NoSuchProcess();
		}
		
		return *it->second;
	}
	
	pid_t GenieProcessTable::NewProcess( Process* process )
	{
		itsProcesses[ itsNextPID ] = boost::shared_ptr< Process >( process );
		
		return itsNextPID++;
	}
	
	int GenieProcessTable::RemoveProcess( iterator it )
	{
		int result = it->second->Result();
		
		itsProcesses.erase( it );
		
		return result;
	}
	
	int GenieProcessTable::RemoveProcess( pid_t pid )
	{
		iterator found = itsProcesses.find( pid );
		
		if ( found == itsProcesses.end() )
		{
			N::SysBeep();
			
			return -1;
		}
		
		return RemoveProcess( found );
	}
	
	void GenieProcessTable::KillAll()
	{
		for ( ProcessMap::iterator it = itsProcesses.begin();  it != itsProcesses.end();  ++it )
		{
			Process& proc = *it->second;
			
			if ( proc.Status() != Process::kTerminated  &&  proc.ProcessID() != 1 )
			{
				proc.Terminate();
			}
		}
		
		Reap();
	}
	
	void GenieProcessTable::Reap()
	{
		std::vector< int > hitlist;
		
		for ( ProcessMap::iterator it = itsProcesses.begin();  it != itsProcesses.end();  ++it )
		{
			Process& proc = *it->second;
			
			int pid = proc.ProcessID();
			
			if (    proc.Status() == Process::kZombie
			     || proc.Status() == Process::kTerminated  &&  proc.ParentProcessID() == 1 )
			{
				hitlist.push_back( pid );
			}
		}
		
		for ( std::vector< int >::iterator it = hitlist.begin();  it != hitlist.end();  ++it )
		{
			int pid = *it;
			(void)RemoveProcess( pid );
		}
	}
	
	void GenieProcessTable::SendSignalToProcessesControlledByTerminal( int sig, TTYHandle* terminal )
	{
		for ( ProcessMap::iterator it = itsProcesses.begin();  it != itsProcesses.end();  ++it )
		{
			Process& proc = *it->second;
			
			if ( proc.ControllingTerminal() == terminal )
			{
				proc.Raise( sig );
			}
		}
	}
	
	void Process::Stop()
	{
		StopThread( itsThread->Get() );
	}
	
	void Process::Continue()
	{
		if ( N::GetThreadState( itsThread->Get() ) == N::kStoppedThreadState )
		{
			N::SetThreadState( itsThread->Get(), N::kReadyThreadState );
		}
	}
	
}

