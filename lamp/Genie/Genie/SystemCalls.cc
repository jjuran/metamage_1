/*	==============
 *	SystemCalls.cc
 *	==============
 */

// Standard C
#include <errno.h>
#include <string.h>

// POSIX
//#include "signal.h"
#include "stdlib.h"
#include "sys/socket.h"
//#include "sys/wait.h"
#include "unistd.h"

// MoreFiles
#include "FileCopy.h"
#include "MoreFilesExtras.h"

// Nitrogen / Carbon
#include "Nitrogen/OpenTransportProviders.h"
#include "Nitrogen/OSStatus.h"

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
	
	
	static int LowestUnusedFrom( const FileDescriptorMap& files, int fd )
	{
		while ( files.find( fd ) != files.end() )
		{
			++fd;
		}
		
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
	
	namespace ext = N::STLExtensions;
	
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
	using Genie::Check_InitOpenTransport;
	using Genie::NewSocket;
	using Genie::kSocketDescriptor;
	using Genie::SocketHandle;
	using Genie::IORef_Cast;
	using Genie::SocketAddress;
	using Genie::FileDescriptor;
	using Genie::Process;
	using Genie::PipeOut_IODetails;
	using Genie::PipeIn_IODetails;
	using Genie::kFileDescriptor;
	using Genie::FileHandle;
	using Genie::kCharacterDeviceDescriptor;
	using Genie::CharacterDevice;
	using Genie::NewPseudoTerminal_Result;
	using Genie::NewPseudoTerminal;
	
	
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
		catch ( Io::NoDataPending& )
		{
			return CurrentProcess().SetErrno( EAGAIN );
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
	
	static int DupFD( int oldfd, int newfd )
	{
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		files[ newfd ] = files[ oldfd ].handle;  // Clears the closeOnExec flag
		
		return newfd;
	}
	
	int dup( int oldfd )
	{
		return DupFD( oldfd, LowestUnusedFrom( CurrentProcess().FileDescriptors(), 0 ) );
	}
	
	int dup2( int oldfd, int newfd )
	{
		return DupFD( oldfd, newfd );
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

