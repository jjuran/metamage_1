/*	===============
 *	GenieProcess.cc
 *	===============
 */

#include "Genie/Process.hh"

// Standard C/C++
#include <csignal>

// Nitrogen core
#include "Nitrogen/Assert.h"

// Nitrogen / Carbon support
#include "Nitrogen/Aliases.h"
#include "Nitrogen/MacErrors.h"
#include "Nitrogen/OSStatus.h"
#include "Nitrogen/Sound.h"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"
#include "Utilities/Threads.h"

// Io
#include "Io/Stream.hh"

// Kerosene/Common
#include "KEnvironment.hh"

// Genie
#include "Genie/pathnames.hh"
#include "Genie/Yield.hh"


#if defined(__MWERKS__) && defined(__csignal__)
	#undef SIGABRT
	#undef SIGFPE
	#undef SIGILL
	#undef SIGINT
	#undef SIGSEGV
	#undef SIGTERM
#endif

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


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NX = NitrogenExtras;
	namespace K = Kerosene;
	
	
	FileDescriptor::FileDescriptor( const FileDescriptor& other )
	:
		handle     ( other.handle      ),
		closeOnExec( other.closeOnExec )
	{
		int goodPlaceToSetABreakPoint = 0;
		if ( goodPlaceToSetABreakPoint );
	}
	
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
		
		context.processContext->Terminate( result );
		
		return result;
	}
	
	struct ExecContext
	{
		FSSpec                      executable;
		std::vector< const char* >  argVector;
		std::string                 pathname;
		
		ExecContext()  {}
		
		ExecContext( const FSSpec&       executable,
		             char const* const*  argv )
		:
			executable( executable ),
			argVector ( argv, argv + Sh::CountStringArray( argv ) + 1 )
		{}
	};
	
	static ExecContext& Normalize( ExecContext& context )
	{
		const OSType type = N::FSpGetFInfo( context.executable ).fdType;
		
		if ( type == 'Wish' )
		{
			return context;  // Already normalized
		}
		
		context.pathname = N::FSpGetPOSIXPathname( context.executable );
		
		if ( type == 'TEXT' )
		{
			// FIXME:  Actually read #! line and select interpreter
			
			const int newTokenCount = 2;
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
			
			context.argVector[ 0 ] = "sh";
			context.argVector[ 1 ] = "--";
			context.argVector[ 2 ] = context.pathname.c_str();  // Overwrite with full pathname
			
			// argv == { "sh", "--", "/usr/bin/script", "foo", "bar", "baz", NULL }
			
			N::FSDirSpec parent = N::FSpGetParent( N::GetProcessAppSpec( N::CurrentProcess() ) );
			context.executable = parent << "Wishes" & "sh";
		}
		else if ( type == 'MPST' )
		{
			context.pathname = N::FSpGetMacPathname( context.executable );
			
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
			
			context.argVector[ 0 ] = "tlsrvr";
			context.argVector[ 1 ] = "--escape";
			context.argVector[ 2 ] = "--";
			context.argVector[ 3 ] = context.pathname.c_str();  // Overwrite with full pathname
			
			// argv == { "sh", "--", "/usr/bin/script", "foo", "bar", "baz", NULL }
			
			N::FSDirSpec parent = N::FSpGetParent( N::GetProcessAppSpec( N::CurrentProcess() ) );
			context.executable = parent << "Wishes" & "tlsrvr";
		}
		else
		{
			throw NotExecutableError();
		}
		
		return context;
	}
	
	static NX::DataPtr< FragmentImage > ReadFragmentImageFromPluginFile( const FSSpec& file )
	{
		N::Owned< N::FSFileRefNum > filehandle = N::FSpOpenDF( file, fsRdPerm );
		
		std::size_t size = N::GetEOF( filehandle );
		
		std::auto_ptr< FragmentImage > result;
		
		result.reset( static_cast< FragmentImage* >( ::operator new( size ) ) );
		
		int bytes = N::FSRead( filehandle, size, reinterpret_cast< char* >( result.get() ) );
		
		return NX::DataPtr< FragmentImage >( result, size );
	}
	
	Process::Process( int ppid ) 
	:
		myPPID              ( ppid ),
		myPID               ( gProcessTable.NewProcess( this ) ),
		fPGID               ( ( ppid > 0 ) ? gProcessTable[ ppid ].GetPGID()
		                                   : 0 ),
		fSID                ( ( ppid > 0 ) ? gProcessTable[ ppid ].GetSID()
		                                   : 0 ),
		myName              ( ppid > 0 ? gProcessTable[ ppid ].ProgramName()
		                               : "init" ),
		myCWD               ( ( ppid > 0 ) ? gProcessTable[ ppid ].CurrentDirectory()
		                                   : N::FSDirSpec() ),
		fControllingTerminal( ( ppid > 0 ) ? gProcessTable[ ppid ].fControllingTerminal
		                                   : NULL ),
		myFileDescriptors   ( ( ppid > 0 ) ? gProcessTable[ ppid ].FileDescriptors()
		                                   : FileDescriptorMap() ),
		myStatus            ( kStarting ),
		fErrnoData          ( NULL ),
		fEnvironData        ( NULL )
	{
	}
	
	void Process::InitThread()
	{
		N::CloseConnection( fOldFragmentConnection );
		fOldFragmentImage = NX::DataPtr< FragmentImage >();
	}
	
	int Process::Exec( const FSSpec&        executable,
	                   const char* const    argv[],
	                   const char* const*   envp )
	{
		// Do we take the name before or after normalization?
		ProgramName( N::Convert< std::string >( executable.name ) );
		
		ExecContext context( executable, argv );
		
		Normalize( context );
		
		FSSpec programFile = context.executable;
		
		// Save the fragment image and the connection that we're running from.
		// We can't use stack storage because we run the risk of the thread terminating.
		fOldFragmentImage      = fFragmentImage;
		fOldFragmentConnection = fFragmentConnection;
		
		fFragmentImage = ReadFragmentImageFromPluginFile( programFile );
		
		fFragmentConnection = N::GetMemFragment< kPrivateCFragCopy >( fFragmentImage.Get(),
		                                                              fFragmentImage.Len() );
		
		K::Versions assumedVersions;
		
		assumedVersions.current        = 2;
		assumedVersions.lastCompatible = 2;
		
		K::Versions* versions = &assumedVersions;
		
		MainProcPtr mainEntryPoint = NULL;
		
		try
		{
			N::FindSymbol( fFragmentConnection, "\p" "main", &mainEntryPoint );
		}
		catch ( ... )
		{
			Io::Stream< IORef > Err = FileDescriptors()[ 2 ].handle;
			
			Err << "Missing main() entry point.\n";
			throw;
		}
		
		try
		{
			N::FindSymbol( fFragmentConnection, "\p" "gVersions", &versions );
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
				Io::Stream< IORef > Err = FileDescriptors()[ 2 ].handle;
				
				Err << "No version data found.\n";
			}
		}
		
		using K::kCurrentVersion;
		using K::kLastCompatibleLibVersion;
		
		if ( kCurrentVersion > versions->current )
		{
			Io::Stream< IORef > Err = FileDescriptors()[ 2 ].handle;
			
			// Plugin interface is older than what we know
			Err << "Plugin uses interface version "
			    << versions->current
			    << "\n";
			
			if ( versions->current < kLastCompatibleLibVersion )
			{
				// It's too old
				Err << "The plugin is too old\n";
				throw N::CFragImportTooOldErr();
			}
		}
		else if ( kCurrentVersion < versions->current )
		{
			Io::Stream< IORef > Err = FileDescriptors()[ 2 ].handle;
			
			// Plugin interface is newer than we know
			Err << "Plugin uses interface version "
			    << versions->current
			    << "\n";
			
			if ( kCurrentVersion < versions->lastCompatible )
			{
				// We're too old
				Err << "The plugin is too new\n";
				throw N::CFragImportTooNewErr();
			}
		}
		
		argvStorage.reset( new Sh::StringArray( &context.argVector[ 0 ] ) );
		
		if ( envp != NULL )
		{
			environStorage.reset( new Sh::StringArray( envp ) );
		}
		else
		{
			environStorage.reset( new Sh::StringArray );
		}
		
		try
		{
			N::FindSymbol( fFragmentConnection, "\p" "environ", &fEnvironData );
			
			*fEnvironData = environStorage->GetPointer();
		}
		catch ( ... ) {}
		
		try
		{
			N::FindSymbol( fFragmentConnection, "\p" "errno", &fErrnoData );
		}
		catch ( ... ) {}
		
		Result( 0 );
		
		ThreadContext threadContext( this,
		                             mainEntryPoint,
		                             argvStorage->GetPointer(),
		                             environStorage->GetPointer() );
		
		// Warning -- if a program calls exec() without forking,
		// then the current thread might be the one we're about to replace.
		// Make sure the assignment is complete before disposing the thread.
		
		std::auto_ptr< Thread > savedThread = thread;
		thread.reset( new Thread( threadContext ) );
		
		Status( Process::kRunning );
		
		savedThread.reset( NULL );  // If this is us, we're toast
		
		return 0;
	}
	
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
	
	int Process::SetErrno( int errorNumber )
	{
		if ( fErrnoData != NULL )
		{
			*fErrnoData = errorNumber;
		}
		
		return -1;
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
			try
			{
				FSSpec spec = ResolveUnixPathname( pathname, myCWD );
				//return ChangeDirectory( N::Convert< N::FSDirSpec >( spec ) );
				myCWD = N::Convert< N::FSDirSpec >( spec );
			}
			catch ( ... )
			{
				Io::S( FileDescriptors()[ 2 ].handle ) << "Genie: cd: " << pathname << ": No such file or directory\n";
				return 1;
			}
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
		myResult = result;
		
		Terminate();
	}
	
	void Process::Orphan()
	{
		ASSERT( myPID != 1 );
		myPPID = 1;
		myFileDescriptors.clear();
	}
	
	sig_t Process::SetSignalAction( int signal, sig_t signalAction )
	{
		if ( signal == SIGKILL  ||  signal == SIGSTOP  ||  signalAction == SIG_ERR )
		{
			//errno = EINVAL;  // FIXME
			return SIG_ERR;
		}
		
		sig_t result = signalMap[ signal ];
		
		signalMap[ signal ] = signalAction;
		
		return result;
	}
	
	void Process::Raise( int signal )
	{
		sig_t action = signalMap[ signal ];
		
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
					Terminate( signal << 8 );
					break;
				case SIGQUIT:
				case SIGILL:
				case SIGTRAP:
				case SIGABRT:
				case SIGEMT:
				case SIGFPE:
				case SIGBUS:
				case SIGSEGV:
				case SIGSYS:
					// create core image
					Terminate( signal << 8 );
					break;
				case SIGCONT:
					// continue process
					/*
					if ( N::GetThreadState( thread ) == kStoppedThreadState )
					{
						N::SetThreadState( thread, kReadyThreadState );
					}
					*/
					// then (fall thru)
				case SIGURG:
				case SIGCHLD:
				case SIGIO:
				case SIGWINCH:
				case SIGINFO:
					// discard signal
					break;
				case SIGSTOP:
				case SIGTSTP:
				case SIGTTIN:
				case SIGTTOU:
					// stop process
					//N::SetThreadState( thread, kStoppedThreadState );
					break;
				default:
					// bad signal
					break;
			}
		}
		else
		{
			// FIXME:  Block this signal during the function call
			action( signal );
		}
	}
	
	GenieProcessTable::GenieProcessTable()
	: 
		myNextPID( 1 )
	{
		new Process( Process::kNoProcessPID );
	}
	
	Process& GenieProcessTable::operator[]( int pid )
	{
		ProcessMap::iterator it = myProcesses.find( pid );
		
		if ( it == myProcesses.end() )
		{
			throw NoSuchProcess();
		}
		
		return *it->second.Get();
	}
	
	int GenieProcessTable::NewProcess( Process* process )
	{
		myProcesses[ myNextPID ] = N::Owned< Process* >::Seize( process );
		return myNextPID++;
	}
	
	int GenieProcessTable::RemoveProcess( iterator it )
	{
		int result = it->second.Get()->Result();
		
		myProcesses.erase( it );
		
		return result;
	}
	
	int GenieProcessTable::RemoveProcess( int pid )
	{
		iterator found = myProcesses.find( pid );
		
		if ( found == myProcesses.end() )
		{
			N::SysBeep();
			return -1;
		}
		
		return RemoveProcess( found );
	}
	
	void GenieProcessTable::KillAll()
	{
		for ( ProcessMap::iterator it = myProcesses.begin();  it != myProcesses.end();  ++it )
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
		
		for ( ProcessMap::iterator it = myProcesses.begin();  it != myProcesses.end();  ++it )
		{
			Process& proc = *it->second;
			
			if ( proc.Status() == Process::kTerminated )
			{
				int pid = proc.ProcessID();
				int ppid = proc.ParentProcessID();
				
				if ( ppid == 1 )
				{
					hitlist.push_back( pid );
				}
			}
		}
		
		for ( std::vector< int >::iterator it = hitlist.begin();  it != hitlist.end();  ++it )
		{
			int pid = *it;
			(void)RemoveProcess( pid );
		}
	}
	
	void GenieProcessTable::SendSignalToProcessesControlledByTerminal( int sig, CharacterDevice* terminal )
	{
		for ( ProcessMap::iterator it = myProcesses.begin();  it != myProcesses.end();  ++it )
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
		//N::SetThreadState( thread->Get(), kStoppedThreadState );
		StopThread( thread->Get() );
	}
	
	void Process::Continue()
	{
		N::SetThreadState( thread->Get(), kReadyThreadState );
	}
	
}

