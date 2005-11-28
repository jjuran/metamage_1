/*	==============
 *	SystemCalls.cc
 *	==============
 */

// Standard C
#include <errno.h>
#include <string.h>

// Standard C++
#include <map>

// POSIX
#include "fcntl.h"
#include "signal.h"
#include "stdlib.h"
#include "sys/select.h"
#include "sys/socket.h"
#include "sys/wait.h"
#include "unistd.h"

// MoreFiles
#include "FileCopy.h"
#include "MoreFilesExtras.h"

// Nitrogen / Carbon
#include "Nitrogen/OpenTransportProviders.h"
#include "Nitrogen/OSStatus.h"
#include "Nitrogen/Timer.h"

// Nitrogen Extras / AEFramework
#include "AEFramework/AEFramework.h"

// POSeven
#include "POSeven/Errno.h"

// Nitrogen Extras / Templates
#include "Templates/FunctionalExtensions.h"
#include "Templates/PointerToFunction.h"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"

// Io
#include "Io/Exceptions.hh"
#include "Io/MakeHandle.hh"
#include "Io/Stream.hh"

// Kerosene/Common
#include "KEnvironment.hh"
#include "SystemCalls.hh"

// Genie
#include "Genie/Devices.hh"
#include "Genie/FileHandle.hh"
#include "Genie/pathnames.hh"
#include "Genie/Pipes.hh"
#include "Genie/Process.hh"
#include "Genie/ReplyHandler.hh"
#include "Genie/ResourceTable.hh"
#include "Genie/SocketHandle.hh"
#include "Genie/Terminal.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	namespace P7 = POSeven;
	
	namespace ext = N::STLExtensions;
	
	
	static int LowestUnusedFrom( const FileDescriptorMap& files, int fd )
	{
		while ( files.find( fd ) != files.end() )
		{
			++fd;
		}
		
		return fd;
	}
	
	static int DuplicateFD( FileDescriptorMap& files, int filedes )
	{
		int fd = LowestUnusedFrom( files, 0 );
		
		files[ fd ] = files[ filedes ].handle;  // Clears the closeOnExec flag
		
		return fd;
	}
	
	struct OTInitialization
	{
		OTInitialization()   { N::InitOpenTransport();  }
		~OTInitialization()  { N::CloseOpenTransport(); }
	};
	
	static void Check_InitOpenTransport()
	{
		static OTInitialization init;
	}
	
	static GenieProcessTable::iterator CheckAnyPID( pid_t ppid )
	{
		bool hasAnyChildren = false;
		
		typedef GenieProcessTable::iterator iterator;
		
		// FIXME:  Replace with find_if
		for ( iterator it = gProcessTable.begin();  it != gProcessTable.end();  ++it )
		{
			Process& proc = *it->second.Get();
			
			if ( proc.ParentProcessID() == ppid )
			{
				if ( proc.Status() == Process::kTerminated )
				{
					return it;
				}
				
				hasAnyChildren = true;
			}
		}
		
		if ( !hasAnyChildren )
		{
			P7::ThrowErrno( ECHILD );
		}
		
		return gProcessTable.end();
	}
	
	static GenieProcessTable::iterator CheckPID( pid_t ppid, pid_t pid )
	{
		typedef GenieProcessTable::iterator iterator;
		
		iterator found = gProcessTable.Map().find( pid );
		
		if ( found == gProcessTable.end() )
		{
			P7::ThrowErrno( ECHILD );
		}
		
		if ( found->second.Get()->ParentProcessID() != ppid )
		{
			// complain
		}
		
		if ( found->second.Get()->Status() != Process::kTerminated )
		{
			found = gProcessTable.end();
		}
		
		return found;
	}
	
	
	/*
	#pragma mark -
	#pragma mark ¥ unistd ¥
	
	static int isatty( K::Environment*, int filedes )
	{
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		IORef ref = files[ filedes ].handle;
		
		return    ref.IsType( kCharacterDeviceDescriptor )
		       && IORef_Cast< CharacterDevice >( ref ).IsATTY();
	}
	*/
	
	#pragma mark -
	#pragma mark ¥ Genie ¥
	
	static void ListOneProcess( const Process* proc, const Io::Stream< IORef >& Out )
	{
		int pid  = proc->ProcessID();
		int ppid = proc->ParentProcessID();
		int pgid = proc->GetPGID();
		int sid  = proc->GetSID();
		
		CharacterDevice* tty = proc->ControllingTerminal();
		
		const char* dev = tty ? tty->DeviceName() : "?";
		
		const std::string& name = proc->ProgramName();
		
		Out << ppid << "  "
		    << pid  << "  "
		    << pgid << "  "
		    << sid  << "  "
		    << dev  << "  "
		    << name << "\n";
	}
	
	static void DoPS( const Io::Stream< IORef >& Out )
	{
		std::for_each
		(
			gProcessTable.begin(), 
			gProcessTable.end(), 
			ext::compose1
			(
				std::bind2nd
				(
					N::PtrFun( ListOneProcess ), 
					Out
				), 
				ext::select2nd< std::pair< int, Process* > >()
			)
		);
	}
	
}

#pragma export on
	
	extern "C"
	{
		void PrintPS();
		
		OSStatus Path2FSSpec( const char*  pathname,
		                      FSSpec*      outFSS );
	}
	
	namespace P7 = POSeven;
	
	using Genie::CurrentProcess;
	using Genie::FileDescriptorMap;
	using Genie::IORef;
	using Genie::gProcessTable;
	using Genie::LowestUnusedFrom;
	using Genie::ResolveUnixPathname;
	using Genie::DuplicateFD;
	using Genie::Check_InitOpenTransport;
	using Genie::NewSocket;
	using Genie::kSocketDescriptor;
	using Genie::SocketHandle;
	using Genie::IORef_Cast;
	using Genie::SocketAddress;
	using Genie::FileDescriptor;
	using Genie::GenieProcessTable;
	using Genie::Process;
	using Genie::CheckAnyPID;
	using Genie::CheckPID;
	using Genie::PipeOut_IODetails;
	using Genie::PipeIn_IODetails;
	using Genie::kFileDescriptor;
	using Genie::FileHandle;
	using Genie::kCharacterDeviceDescriptor;
	using Genie::CharacterDevice;
	using Genie::NewPseudoTerminal_Result;
	using Genie::NewPseudoTerminal;
	using Genie::OpenFile;
	using Genie::kPollRead;
	using Genie::kPollWrite;
	using Genie::kPollExcept;
	
	
	int* ErrnoPtr()
	{
		return CurrentProcess().ErrnoData();
	}
	
	char*** EnvironPtr()
	{
		return CurrentProcess().EnvironData();
	}
	
	OSStatus AESendBlocking( const AppleEvent* appleEvent, AppleEvent* reply )
	{
		try
		{
			(void)N::AESend( *appleEvent,
			                 kAEQueueReply | kAECanInteract,
			                 kAENormalPriority );
			
			// Now that we've sent the event, retrieve the return ID
			N::AEReturnID returnID = N::AEGetAttributePtr< typeSInt32 >( *appleEvent, keyReturnIDAttr );
			
			// Subscribe to AEFramework's queued reply delivery and wake-up service
			N::ExpectReply( returnID, reply );
			
			// Sleep until the reply is delivered
			pause();
		}
		catch ( N::OSStatus& err )
		{
			return err;
		}
		
		return noErr;
	}
	
	InetSvcRef InternetServices()
	{
		Check_InitOpenTransport();
		
		static N::Owned< InetSvcRef >
		       gInetSvcRef = N::OTOpenInternetServices( kDefaultInternetServicesPath );
		
		return gInetSvcRef;
	}
	
	void PrintPS()
	{
		using Genie::DoPS;
		
		DoPS( Io::Stream< IORef >( CurrentProcess().FileDescriptors()[ 1 ].handle ) );
	}
	
	OSStatus Path2FSSpec( const char*  pathname,
	                      FSSpec*      outFSS )
	{
		using Genie::ResolveUnixPathname;
		
		try
		{
			*outFSS = ResolveUnixPathname( pathname, CurrentProcess().CurrentDirectory() );
		}
		catch ( N::OSStatus err )
		{
			return CurrentProcess().SetErrno( err );
		}
		
		return 0;
	}
	
	#pragma mark -
	#pragma mark ¥ signal ¥
	
	int kill( pid_t pid, int sig )
	{
		if ( !gProcessTable.Exists( pid ) )
		{
			return -1;  // FIXME:  set errno
		}
		
		if ( sig != 0 )
		{
			gProcessTable[ pid ].Raise( sig );
		}
		
		return 0;
	}
	
	__sig_handler signal( int sig, __sig_handler func )
	{
		return CurrentProcess().SetSignalAction( sig, func );
	}
	
	#pragma mark -
	#pragma mark ¥ stdlib ¥
	
	char* getenv( const char* name )
	{
		return CurrentProcess().GetEnv( name );
	}
	

	int setenv( const char* name, const char* value, int overwrite )
	{
		CurrentProcess().SetEnv( name, value, overwrite );
		
		return 0;
	}
	
	int putenv( const char* string )
	{
		CurrentProcess().PutEnv( string );
		
		return 0;
	}
	
	void unsetenv( const char* name )
	{
		CurrentProcess().UnsetEnv( name );
	}
	
	int clearenv()
	{
		CurrentProcess().ClearEnv();
		
		return 0;
	}
	
	#pragma mark -
	#pragma mark ¥ fcntl ¥
	
	int open( const char* path, int oflag, mode_t mode )
	{
		try
		{
			FileDescriptorMap& files = CurrentProcess().FileDescriptors();
			
			int fd = LowestUnusedFrom( files, 0 );
			
			unsigned char rdPerm = oflag + 1  &  FREAD;
			unsigned char wrPerm = oflag + 1  &  FWRITE;
			
			bool nonblocking = oflag & O_NONBLOCK;
			bool appending   = oflag & O_APPEND;
			// ...
			bool creating    = oflag & O_CREAT;
			bool truncating  = oflag & O_TRUNC;
			bool excluding   = oflag & O_EXCL;
			// ...
			bool resFork     = oflag & O_ResFork;
			bool resMap      = oflag & O_ResMap;
			
			std::string pathname = path;
			
			IORef io;
			
			if ( pathname.substr( 0, 5 ) == "/dev/" )
			{
				io = Genie::GetSimpleDeviceHandle( path );
			}
			else
			{
				// assume it's a file
				FSSpec file = ResolveUnixPathname( path,
				                                   N::Convert< N::FSDirSpec >( CurrentProcess().CurrentDirectory() ) );
				
				if ( creating )
				{
					if ( !N::FSpTestFileExists( file ) )
					{
						// FIXME:  Need a way to guess or otherwise choose creator and type
						// Cheap hack:
						N::OSType creator = TARGET_API_MAC_CARBON ? '!Rch' : 'R*ch';
						N::FSpCreate( file, creator, 'TEXT' );
					}
					else if ( excluding )
					{
						// error creating
						throw N::DupFNErr();
					}
				}
				
				N::Owned< N::FSFileRefNum > fileH = N::FSpOpenDF( file, rdPerm | wrPerm );
				
				if ( truncating )
				{
					N::SetEOF( fileH, 0 );
				}
				else if ( appending )
				{
					N::SetFPos( fileH, fsFromLEOF, 0 );
				}
				
				io = OpenFile( fileH );
			}
			
			files[ fd ] = io;
			
			return fd;
		}
		catch ( N::OSStatus& err )
		{
			// FIXME
			return CurrentProcess().SetErrno( EINVAL );
		}
	}
	
	int fcntl( int filedes, int cmd, int param )
	{
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		switch ( cmd )
		{
			case F_DUPFD:
				return DuplicateFD( files, filedes );
			
			case F_GETFD:
				return files[ filedes ].closeOnExec;
			
			case F_SETFD:
				files[ filedes ].closeOnExec = param;
				return 0;
			
			/*
			case F_GETFL:
			case F_SETFL:
				break;
			*/
			
			// Disabled until we figure out how to model nonblocking mode
			/*
			case F_GetFlag:
				if ( param == O_NONBLOCK )
				{
					//return IsNonBlocking();
				}
				break;
			
			case F_SetFlag:
				if ( param == O_NONBLOCK )
				{
					files[ filedes ].handle.Get()->SetNonBlocking();
					return 0;
				}
				break;
			
			case F_ClearFlag:
				if ( param == O_NONBLOCK )
				{
					files[ filedes ].handle.Get()->SetBlocking();
					return 0;
				}
				break;
			*/
			
			default:
				break;
		}
		
		return CurrentProcess().SetErrno( EINVAL );
	}
	
	#pragma mark -
	#pragma mark ¥ sys/select ¥
	
	int select( int n, fd_set*  readfds,
	                   fd_set*  writefds,
	                   fd_set*  exceptfds, struct timeval* timeout )
	{
		UInt64 timeToBail = timeout == NULL ? UInt64( -1 )
		                                    : N::Microseconds() + timeout->tv_sec * 1000000
		                                                        + timeout->tv_usec;
		
		try
		{
			FileDescriptorMap& files = CurrentProcess().FileDescriptors();
			
			// Output fd sets
			fd_set rd, wr, ex;
			
			FD_ZERO( &rd );
			FD_ZERO( &wr );
			FD_ZERO( &ex );
			
			// For any omitted sets, we'll pretend they were zeroed.
			
			if ( readfds   == NULL )  { readfds   = &rd; }
			if ( writefds  == NULL )  { writefds  = &wr; }
			if ( exceptfds == NULL )  { exceptfds = &ex; }
			
			int result = 0;
			
			do
			{
				Genie::Yield();
				
				for ( int i = 0;  i != n;  ++i )
				{
					if (    FD_ISSET( i, readfds   )
					     || FD_ISSET( i, writefds  )
					     || FD_ISSET( i, exceptfds ) )
					{
						if ( !files.count( i ) )
						{
							return CurrentProcess().SetErrno( EBADF );
						}
						
						IORef ref = files[ i ].handle;
						
						unsigned int poll = ref.Poll();
						
						//if ( poll == 0 )  continue;  // Optimization
						
						if ( FD_ISSET( i, readfds )  &&  poll & kPollRead )
						{
							FD_SET( i, &rd );
							
							++result;
						}
						
						if ( FD_ISSET( i, writefds )  &&  poll & kPollWrite )
						{
							FD_SET( i, &wr );
							
							++result;
						}
						
						if ( FD_ISSET( i, exceptfds )  &&  poll & kPollExcept )
						{
							FD_SET( i, &ex );
							
							++result;
						}
					}
				}
			}
			while ( result == 0  &&  N::Microseconds() < timeToBail );
			
			if ( n > 0 )
			{
				*readfds   = rd;
				*writefds  = wr;
				*exceptfds = ex;
			}
			
			return result;
		}
		catch ( P7::Errno& error )
		{
			CurrentProcess().SetErrno( error );
		}
		catch ( ... )
		{
			// FIXME:  Is there a better course of action, or at least a better default?
			CurrentProcess().SetErrno( EIO );
		}
		
		return -1;
	}
	
	#pragma mark -
	#pragma mark ¥ sys/socket ¥
	
	#if !TARGET_CPU_68K
	
	int socket( int domain, int type, int protocol )
	{
		Check_InitOpenTransport();
		
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		int fd = LowestUnusedFrom( files, 0 );
		
		// Assume domain is PF_INET, type is SOCK_STREAM, and protocol is INET_TCP
		
		try
		{
			files[ fd ] = NewSocket();
		}
		catch ( ... )
		{
			// convert exception
			return -1;
		}
		
		return fd;
	}
	
	int bind( int sockfd, const struct sockaddr* name, socklen_t namelen )
	{
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		const InetAddress* inetAddress = reinterpret_cast< const InetAddress* >( name );
		
		try
		{
			// FIXME:  Verify socket exists first
			if ( files[ sockfd ].handle.IsType( kSocketDescriptor ) )
			{
				SocketHandle& sock = IORef_Cast< SocketHandle >( files[ sockfd ].handle );
				sock.Bind( inetAddress, namelen );
			}
			else
			{
				return CurrentProcess().SetErrno( ENOTSOCK );
			}
		}
		catch ( ... )
		{
			// Convert the exception
			return -1;
		}
		
		return 0;
	}
	
	int listen( int sockfd, int backlog )
	{
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		try
		{
			// FIXME:  Verify socket exists first
			if ( files[ sockfd ].handle.IsType( kSocketDescriptor ) )
			{
				SocketHandle& sock = IORef_Cast< SocketHandle >( files[ sockfd ].handle );
				sock.Listen( backlog );
			}
			else
			{
				return CurrentProcess().SetErrno( ENOTSOCK );
			}
		}
		catch ( ... )
		{
			// Convert the exception
			return -1;
		}
		
		return 0;
	}
	
	int accept( int sockfd, struct sockaddr *addr, socklen_t *addrlen )
	{
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		try
		{
			// FIXME:  Verify socket exists first
			if ( files[ sockfd ].handle.IsType( kSocketDescriptor ) )
			{
				SocketHandle& sock = IORef_Cast< SocketHandle >( files[ sockfd ].handle );
				
				//Accept_Result result = sock->Accept();
				
				IORef handle = sock.Accept( (InetAddress*)addr, *addrlen );
				
				int fd = LowestUnusedFrom( files, 0 );
				
				files[ fd ] = FileDescriptor( handle );
				
				return fd;
			}
			else
			{
				return CurrentProcess().SetErrno( ENOTSOCK );
			}
		}
		catch ( ... )
		{
			// Convert the exception
			return -1;
		}
		
		return 0;
	}
	
	int connect( int sockfd, const struct sockaddr* serv_addr, socklen_t addrlen )
	{
		// Assume sin_family is AF_INET
		
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		const InetAddress* inetAddress = reinterpret_cast< const InetAddress* >( serv_addr );
		
		try
		{
			// FIXME:  Verify socket exists first
			if ( files[ sockfd ].handle.IsType( kSocketDescriptor ) )
			{
				SocketHandle& sock = IORef_Cast< SocketHandle >( files[ sockfd ].handle );
				sock.Connect( inetAddress, addrlen );
			}
			else
			{
				return CurrentProcess().SetErrno( ENOTSOCK );
			}
		}
		catch ( ... )
		{
			// Convert the exception
			return -1;
		}
		
		return 0;
	}
	
	int getsockname( int sockfd, struct sockaddr* name, socklen_t* namelen )
	{
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		try
		{
			// FIXME:  Verify socket exists first
			if ( files[ sockfd ].handle.IsType( kSocketDescriptor ) )
			{
				SocketHandle& sock = IORef_Cast< SocketHandle >( files[ sockfd ].handle );
				
				const SocketAddress& address = sock.GetSockName();
				
				*namelen = address.Len();
				std::memcpy( name, address.Get(), address.Len() );
			}
			else
			{
				return CurrentProcess().SetErrno( ENOTSOCK );
			}
		}
		catch ( ... )
		{
			// Convert the exception
			return -1;
		}
		
		return 0;
	}
	
	int getpeername( int sockfd, struct sockaddr* name, socklen_t* namelen )
	{
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		try
		{
			// FIXME:  Verify socket exists first
			if ( files[ sockfd ].handle.IsType( kSocketDescriptor ) )
			{
				SocketHandle& sock = IORef_Cast< SocketHandle >( files[ sockfd ].handle );
				
				const SocketAddress& address = sock.GetPeerName();
				
				*namelen = address.Len();
				std::memcpy( name, address.Get(), address.Len() );
			}
			else
			{
				return CurrentProcess().SetErrno( ENOTSOCK );
			}
		}
		catch ( ... )
		{
			// Convert the exception
			return -1;
		}
		
		return 0;
	}
	
	#endif
	
	#pragma mark -
	#pragma mark ¥ sys/wait ¥
	
	pid_t waitpid( pid_t pid, int* stat_loc, int options )
	{
		int ppid = CurrentProcess().ProcessID();
		
		try
		{
			while ( true )
			{
				typedef GenieProcessTable::iterator iterator;
				
				iterator found = pid == -1 ? CheckAnyPID( ppid )
				                           : CheckPID( ppid, pid );
				
				if ( found != gProcessTable.end() )
				{
					if ( stat_loc != NULL )
					{
						*stat_loc = found->second.Get()->Result();
					}
					
					int foundPID = found->first;
					
					(void)gProcessTable.RemoveProcess( found );
					
					return foundPID;
				}
				
				if ( options & 1 )
				{
					// no hang
					return 0;
				}
				
				Genie::Yield();
			}
		}
		catch ( P7::Errno& err )
		{
			CurrentProcess().SetErrno( err );
		}
		
		return -1;
	}
	
	#pragma mark -
	#pragma mark ¥ unistd ¥
	
	int chdir( const char* path )
	{
		try
		{
			int result = Genie::CurrentProcess().ChangeDirectory( path );
			
			return result == 0 ? 0 : -1;
		}
		catch ( N::FNFErr& )
		{
			return CurrentProcess().SetErrno( ENOENT );
		}
		catch ( ... ) {}
		
		return CurrentProcess().SetErrno( EINVAL );
	}
	
	int close( int fd )
	{
		Genie::CurrentProcess().FileDescriptors().erase( fd );
		
		return 0;
	}
	
	static void FSpFileCopy( const FSSpec&         source,
	                         const FSSpec&         destDir,
	                         const unsigned char*  copyName       = NULL,
	                         void*                 copyBufferPtr  = NULL,
	                         long                  copyBufferSize = 0,
	                         bool                  preflight      = true )
	{
		N::OnlyOnce< N::RegisterFileManagerErrors >();
		
		N::ThrowOSStatus( ::FSpFileCopy( &source,
		                                 &destDir,
		                                 copyName,
		                                 copyBufferPtr,
		                                 copyBufferSize,
		                                 preflight ) );
	}
	
	int copyfile( const char* src, const char* dest )
	{
		try
		{
			const N::FSDirSpec cwd = CurrentProcess().CurrentDirectory();
			
			const FSSpec srcFile  = ResolveUnixPathname( src,  cwd );
			const FSSpec destFile = ResolveUnixPathname( dest, cwd );
			
			FSpFileCopy( srcFile, destFile );
		}
		catch ( ... )
		{
			return CurrentProcess().SetErrno( EINVAL );
		}
		
		return 0;
	}
	
	int dup( int oldfd )
	{
		return DuplicateFD( CurrentProcess().FileDescriptors(), oldfd );
	}
	
	int dup2( int oldfd, int newfd )
	{
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		files[ newfd ] = files[ oldfd ].handle;  // Clears the closeOnExec flag
		
		return newfd;
	}
	
	int execve( const char* path, const char* const argv[], const char* const envp[] )
	{
		try
		{
			Process& current( CurrentProcess() );
			
			FSSpec progFile = ResolveUnixPathname( path, current.CurrentDirectory() );
			
			if ( current.Forked() )
			{
				Process* parent = &gProcessTable[ current.ParentProcessID() ];
				
				// start a new thread with the child's process context
				current.Exec( progFile, argv, envp );
				
				// Hope nothing bad happened while we thought we were still the child
				
				// set this thread's process context back to the forker
				RegisterProcessContext( parent );
				
				// Yes, in Genie a forked exec() DOES return on success.
				return 0;
				
				// For vfork hack:
				//return childPID;  // user will longjmp()
			}
			else
			{
				// save thread object
				// replace process context's thread with a new one
				// dispose old thread
				current.Exec( progFile, argv, envp );  // shouldn't return
				
				return 0;  // just in case
			}
		}
		catch ( N::OSStatus& err )
		{
			std::string errMsg = "\n";
			
			try
			{
				throw;
			}
			catch ( N::ErrMessage& msg )
			{
				errMsg = ", errMessage: " + N::Convert< std::string >( msg.errMessage ) + "\n";
				//std::printf( "errMessage: '%s'\n", str.c_str() );
			}
			catch ( N::FNFErr& )
			{
				return CurrentProcess().SetErrno( ENOENT );
			}
			catch ( ... )  {}
			
			std::printf( "OSStatus %d%s", int( err.Get() ), errMsg.c_str() );
			
			// This is wrong -- we should return a unix error, not an OSStatus.
			return CurrentProcess().SetErrno( EINVAL );
		}
		catch ( ... )
		{
			return -1;
		}
		
		return 0;
	}
	
	void _exit( int status )
	{
		Process& current( CurrentProcess() );
		
		current.Terminate( (status & 0xFF) << 8 );  // doesn't reap, won't terminate thread
		
		if ( current.Forked() )
		{
			RegisterProcessContext( &gProcessTable[ current.ParentProcessID() ] );
			
			return;
		}
		
		current.KillThread();
	}
	
	char* getcwd( char* buf, std::size_t size )
	{
		std::string result = N::FSpGetPOSIXPathname( N::Convert< FSSpec >( CurrentProcess().CurrentDirectory() ) );
		
		if ( result.size() + 1 > size )
		{
			return NULL;
		}
		
		std::copy( result.c_str(),
		           result.c_str() + result.size() + 1,
		           buf );
		
		return buf;
	}
	
	pid_t getpgid( pid_t pid )
	{
		if ( pid == 0 )
		{
			return getpgrp();
		}
		
		return Genie::gProcessTable[ pid ].GetPGID();
	}
	
	pid_t getpgrp()
	{
		return Genie::CurrentProcess().GetPGID();
	}
	
	pid_t getpid()
	{
		return Genie::CurrentProcess().ProcessID();
	}
	
	pid_t getppid()
	{
		return Genie::CurrentProcess().ParentProcessID();
	}
	
	off_t lseek( int fd, off_t offset, int whence )
	{
		
		
		try
		{
			FileDescriptorMap& files = CurrentProcess().FileDescriptors();
			
			IORef ref = files[ fd ].handle;
			
			if ( !ref.IsType( kFileDescriptor ) )
			{
				return CurrentProcess().SetErrno( ESPIPE );
			}
			
			FileHandle& fh = IORef_Cast< FileHandle >( ref );
			
			N::FSFileRefNum refNum = fh.GetRefNum();
			
			N::FSIOPosMode mode;
			
			switch ( whence )
			{
				case SEEK_SET:
					mode = fsFromStart;
					break;
				
				case SEEK_CUR:
					mode = fsFromMark;
					break;
				
				case SEEK_END:
					mode = fsFromLEOF;
					break;
				
				default:
					return CurrentProcess().SetErrno( EINVAL );
			}
			
			N::SetFPos( refNum, mode, offset );
			
			return N::GetFPos( refNum );
		}
		catch ( ... ) {}
		
		return CurrentProcess().SetErrno( EINVAL );
	}
	
	int pause()
	{
		Genie::CurrentProcess().Stop();  // Sleep, until...
		
		return CurrentProcess().SetErrno( EINTR );
	}
	
	int pipe( int filedes[ 2 ] )
	{
		using Genie::Pipe;
		
		Pipe::Handles handles = Pipe::New();
		
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		int reader = LowestUnusedFrom( files, 3 );
		int writer = LowestUnusedFrom( files, reader + 1 );
		
		files[ reader ] = Io::MakeHandleFromCopy< PipeOut_IODetails >( handles.out );
		files[ writer ] = Io::MakeHandleFromCopy< PipeIn_IODetails  >( handles.in  );
		
		filedes[ 0 ] = reader;
		filedes[ 1 ] = writer;
		
		return 0;
	}
	
	int peek( int fd, const char** buffer, size_t minBytes )
	{
		try
		{
			FileDescriptorMap& files = CurrentProcess().FileDescriptors();
			
			if ( !files.count( fd ) )
			{
				CurrentProcess().SetErrno( EBADF );
				return NULL;
			}
			
			const std::string& peekBuffer = files[ fd ].handle.Peek( minBytes );
			
			if ( buffer != NULL )
			{
				*buffer = peekBuffer.c_str();
			}
			
			return peekBuffer.size();
		}
		catch ( Io::EndOfInput )
		{
			// Zero return from peek() indicates EOF
			return 0;
		}
		catch ( Io::NoDataPending )
		{
			CurrentProcess().SetErrno( EWOULDBLOCK );
		}
		catch ( P7::Errno& error )
		{
			CurrentProcess().SetErrno( error );
		}
		catch ( ... )
		{
			// FIXME:  Is there a better course of action, or at least a better default?
			CurrentProcess().SetErrno( EIO );
		}
		
		return -1;
	}
	
	ssize_t read( int fd, void* buf, size_t count )
	{
		try
		{
			FileDescriptorMap& files = CurrentProcess().FileDescriptors();
			
			if ( !files.count( fd ) )
			{
				return CurrentProcess().SetErrno( EBADF );
			}
			
			int get = Io::Read( files[ fd ].handle,
			                    reinterpret_cast< char* >( buf ),
			                    count );
			return get;
		}
		catch ( Io::EndOfInput )
		{
			// Zero return from read() indicates EOF
			return 0;
		}
		catch ( Io::NoDataPending )
		{
			CurrentProcess().SetErrno( EWOULDBLOCK );
		}
		catch ( P7::Errno& error )
		{
			CurrentProcess().SetErrno( error );
		}
		catch ( ... )
		{
			// FIXME:  Is there a better course of action, or at least a better default?
			CurrentProcess().SetErrno( EIO );
		}
		
		return -1;
	}
	
	// setctty() makes fd the controlling terminal of the current process.
	// I made this up.
	int setctty( int fd )
	{
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		IORef ref = files[ fd ].handle;
		
		if ( ref.IsType( kCharacterDeviceDescriptor ) )
		{
			CharacterDevice& dev = IORef_Cast< CharacterDevice >( ref );
			
			if ( dev.IsATTY() )
			{
				CurrentProcess().SetControllingTerminal( &dev );
				
				return 0;
			}
		}
		
		CurrentProcess().SetErrno( EINVAL );
		return -1;
	}
	
	int setpgid( pid_t pid, pid_t pgid )
	{
		Process& target( pid != 0 ? gProcessTable[ pid ]
		                          : CurrentProcess() );
		
		pid = target.ProcessID();
		
		target.SetPGID( pgid != 0 ? pgid : pid );
		
		return pid;
	}
	
	pid_t setsid()
	{
		Process& current( CurrentProcess() );
		
		current.SetControllingTerminal( NULL );
		
		int pid = current.ProcessID();
		
		current.SetPGID( pid );
		current.SetSID ( pid );
		
		return pid;
	}
	
	unsigned int sleep( unsigned int seconds )
	{
		unsigned long ticks = ::TickCount();
		
		ticks += seconds * 60;  // Close enough
		
		try
		{
			// Yield at least once, even for 0 seconds
			do
			{
				Genie::Yield();
			}
			while ( ::TickCount() < ticks );
		}
		catch ( P7::Errno& error )
		{
			return CurrentProcess().SetErrno( error );
		}
		
		return 0;
	}
	
	const char* ttyname( int fd )
	{
		try
		{
			FileDescriptorMap& files = CurrentProcess().FileDescriptors();
			
			IORef ref = files[ fd ].handle;
			
			if ( ref.IsType( kCharacterDeviceDescriptor ) )
			{
				CharacterDevice& dev = IORef_Cast< CharacterDevice >( ref );
				
				if ( dev.IsATTY() )
				{
					return dev.DeviceName();
				}
			}
		}
		catch ( ... ) {}
		
		return NULL;
	}
	
	// ttypair() is analogous to socketpair(), and creates a pseudo-tty device.
	// File descriptors refering to the master and slave respectively are returned
	// in filedes.
	// I made this up too.
	int ttypair( int filedes[ 2 ] )
	{
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		NewPseudoTerminal_Result fds = NewPseudoTerminal();
		
		int fd0 = LowestUnusedFrom( files, 0 );
		int fd1 = LowestUnusedFrom( files, fd0 + 1 );
		
		files[ fd0 ] = fds.first;
		files[ fd1 ] = fds.second;
		
		filedes[ 0 ] = fd0;
		filedes[ 1 ] = fd1;
		
		return 0;
	}
	
	ssize_t write( int fd, const void* buf, size_t count )
	{
		int put = Io::Write( Genie::CurrentProcess().FileDescriptors()[ fd ].handle,
		                     reinterpret_cast< const char* >( buf ),
		                     count );
		return put;
	}
	
#pragma export reset

