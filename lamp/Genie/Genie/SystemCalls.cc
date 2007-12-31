/*	==============
 *	SystemCalls.cc
 *	==============
 */

// Standard C
#include <errno.h>
#include <signal.h>
#include <string.h>

// POSIX
#include "sys/ioctl.h"
#include "sys/stat.h"
#include "unistd.h"

// Nucleus
#include "Nucleus/NAssert.h"

// Io
#include "io/io.hh"

// Nitrogen
#include "Nitrogen/OSStatus.h"

// Nitrogen Extras / AEFramework
#include "AEFramework/AEFramework.h"

// POSeven
#include "POSeven/Errno.hh"

// OSErrno
#include "OSErrno/OSErrno.hh"

// Kerosene/Common
#include "KEnvironment.hh"
#include "SystemCalls.hh"

// Genie
#include "Genie/FileDescriptors.hh"
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/IO/Pipe.hh"
#include "Genie/IO/PseudoTTY.hh"
#include "Genie/IO/RegularFile.hh"
#include "Genie/IO/Terminal.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"
#include "Genie/Yield.hh"


extern "C"
{
	OSStatus Path2FSSpec( const char*  pathname,
	                      FSSpec*      outFSS );
}

namespace Nucleus
{
	
	template <>
	struct Converter< poseven::errno_t, io::end_of_input > : public std::unary_function< io::end_of_input, poseven::errno_t >
	{
		poseven::errno_t operator()( io::end_of_input ) const
		{
			return poseven::errno_t( 0 );
		}
	};
	
}

namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	namespace p7 = poseven;
	
	
	static p7::errno_t GetErrnoFromException()
	{
		NN::RegisterExceptionConversion< p7::errno_t, N::OSStatus          >();
		NN::RegisterExceptionConversion< p7::errno_t, io::end_of_input     >();
		NN::RegisterExceptionConversion< p7::errno_t, io::no_input_pending >();
		
		p7::errno_t errnum = NN::Convert< p7::errno_t >( NN::TheExceptionBeingHandled(), EINVAL );
		
		return errnum;
	}
	
	static int SetErrnoFromExceptionInSystemCall( Process& process )
	{
		p7::errno_t errnum = GetErrnoFromException();
		
		return process.SetErrno( errnum );
	}
	
	int SetErrnoFromExceptionInSystemCall()
	{
		return SetErrnoFromExceptionInSystemCall( CurrentProcess() );
	}
	
	
	SystemCallFrame::SystemCallFrame( const char* name ) : itsCaller( CurrentProcess() ),
	                                                       itsName  ( name ),
	                                                       itsErrno ( 0 )
	{
		itsCaller.EnterSystemCall( name );
	}
	
	SystemCallFrame::~SystemCallFrame()
	{
		itsCaller.LeaveSystemCall();
	}
	
	int SystemCallFrame::SetErrno( int errorNumber )
	{
		itsErrno = errorNumber;
		
		return itsCaller.SetErrno( errorNumber );
	}
	
	int SystemCallFrame::SetErrnoFromException()
	{
		return SetErrno( GetErrnoFromException() );
	}
	
	
	#pragma mark -
	#pragma mark ¥ Genie ¥
	
	static void InitProc( CleanupHandlerProc cleanup, int* errno_addr )
	{
		Process& current = CurrentProcess();
		
		current.SetCleanupHandler( cleanup );
		
		current.SetErrnoPtr( errno_addr );
	}
	
	REGISTER_SYSTEM_CALL( InitProc );
	
	static OSStatus AESendBlocking( const AppleEvent* appleEvent, AppleEvent* reply )
	{
		SystemCallFrame frame( "AESendBlocking" );
		
		try
		{
			(void) N::AESend( *appleEvent, N::kAEQueueReply | N::kAECanInteract );
			
			// Now that we've sent the event, retrieve the return ID
			N::AEReturnID returnID = N::AEGetAttributePtr< N::keyReturnIDAttr >( *appleEvent );
			
			// Subscribe to AEFramework's queued reply delivery and wake-up service
			N::ExpectReply( returnID, reply );
			
			// Sleep until the reply is delivered
			frame.Caller().Raise( SIGSTOP );
		}
		catch ( const N::OSStatus& err )
		{
			return err;
		}
		
		return noErr;
	}
	
	REGISTER_SYSTEM_CALL( AESendBlocking );
	
	static OSStatus Path2FSSpec( const char*  pathname,
	                             FSSpec*      outFSS )
	{
		SystemCallFrame frame( "Path2FSSpec" );
		
		try
		{
			*outFSS = ResolvePathname( pathname, CurrentProcess().GetCWD() )->GetFSSpec();
		}
		catch ( const N::OSStatus& err )
		{
			return err;
		}
		catch ( ... )
		{
			return extFSErr;  // Just a guess
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( Path2FSSpec );
	
	
	#pragma mark -
	#pragma mark ¥ unistd ¥
	
	static unsigned int alarm( unsigned int seconds )
	{
		SystemCallFrame frame( "alarm" );
		
		return CurrentProcess().SetAlarm( seconds );
	}
	
	REGISTER_SYSTEM_CALL( alarm );
	
	static int chdir( const char* pathname )
	{
		SystemCallFrame frame( "chdir" );
		
		try
		{
			if ( pathname == NULL )
			{
				return frame.SetErrno( EINVAL );
			}
			
			FSTreePtr newCWD = ResolvePathname( pathname, CurrentProcess().GetCWD() );
			
			if ( newCWD->IsLink() )
			{
				newCWD = newCWD->ResolveLink();
			}
			
			CurrentProcess().ChangeDirectory( newCWD );
			
			return 0;
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
	}
	
	REGISTER_SYSTEM_CALL( chdir );
	
	
	static int close( int fd )
	{
		SystemCallFrame frame( "close" );
		
		try
		{
			CloseFileDescriptor( fd );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( close );
	
	static int dup2( int oldfd, int newfd );
	
	static int dup( int oldfd )
	{
		return dup2( oldfd, LowestUnusedFileDescriptor() );
	}
	
	REGISTER_SYSTEM_CALL( dup );
	
	static int dup2( int oldfd, int newfd )
	{
		SystemCallFrame frame( "dup2" );
		
		try
		{
			return DuplicateFileDescriptor( oldfd, newfd );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
	}
	
	REGISTER_SYSTEM_CALL( dup2 );
	
	static int execve( const char* path, const char* const argv[], const char* const envp[] )
	{
		// On a successful exec, the frame doesn't get destructed, but we compensate.
		// Exec() calls Suspend(), which is equivalent to LeaveSystemCall().
		// ResumeAfterFork() calls Resume() and LeaveSystemCall().
		SystemCallFrame frame( "execve" );
		
		try
		{
			Process& current( CurrentProcess() );
			
			bool forked = current.Forked();
			
			NN::Owned< N::ThreadID > savedThread;
			
			// Local scope to make sure progFile gets destroyed
			{
				FSTreePtr progFile = ResolvePathname( path, current.GetCWD() );
				
				ResolveLinks_InPlace( progFile );
				
				struct ::stat sb;
				
				progFile->Stat( sb );
				
				if ( (sb.st_mode & S_IXUSR) == 0 )
				{
					return frame.SetErrno( EACCES );
				}
				
				// Start a new thread with the child's process context
				savedThread = current.Exec( progFile, argv, envp );
			}
			
			// If this thread is us (i.e. we didn't fork), we're now toast.
			if ( savedThread.Get() != N::kNoThreadID )
			{
				N::DisposeThread( savedThread, NULL, false );
			}
			
			// A non-forked exec kills its own thread and doesn't return
			ASSERT( forked );
			
			// Hope nothing bad happened while we thought we were still the child
			
			// Have the parent longjmp() back into vfork() and resume
			GetProcess( current.GetPPID() ).ResumeAfterFork();
			
			// Not reached
		}
		catch ( const N::OSStatus& err )
		{
			if ( err.Get() == resNotFound )
			{
				return frame.SetErrno( ENOEXEC );
			}
			
			std::string errMsg = "\n";
			
			try
			{
				throw;
			}
			catch ( const N::ErrMessage& msg )
			{
				errMsg = ", errMessage: " + NN::Convert< std::string >( msg.errMessage ) + "\n";
			}
			catch ( ... )  {}
			
			if ( err.Get() != fnfErr )
			{
				std::printf( "OSStatus %d%s", int( err.Get() ), errMsg.c_str() );
			}
			
			return frame.SetErrnoFromException();
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		// Not reached
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( execve );
	
	static void _exit( int status )
	{
		// The frame doesn't get destructed, but we compensate.
		// ResumeAfterFork() calls Resume() and LeaveSystemCall().
		SystemCallFrame frame( "_exit" );
		
		Process& current( CurrentProcess() );
		
		current.Exit( status );  // doesn't return unless forked
		
		ASSERT( current.Forked() );
		
		GetProcess( current.GetPPID() ).ResumeAfterFork();  // doesn't return
	}
	
	REGISTER_SYSTEM_CALL( _exit );
	
	static ssize_t getcwd_k( char* buffer, std::size_t buffer_size )
	{
		SystemCallFrame frame( "getcwd_k" );
		
		try
		{
			FSTreePtr cwd = CurrentProcess().GetCWD();
			
			std::string result = cwd->Pathname();
			
			buffer_size = std::min( buffer_size, result.size() );
			
			std::copy( result.c_str(),
			           result.c_str() + result.size(),
			           buffer );
			
			return result.size();
		}
		catch ( ... )
		{
			return NULL;
		}
	}
	
	REGISTER_SYSTEM_CALL( getcwd_k );
	
	static pid_t getpgrp();
	
	static pid_t getpgid( pid_t pid )
	{
		SystemCallFrame frame( "getpgid" );
		
		if ( pid == 0 )
		{
			return getpgrp();
		}
		
		return GetProcess( pid ).GetPGID();
	}
	
	REGISTER_SYSTEM_CALL( getpgid );
	
	static pid_t getpgrp()
	{
		SystemCallFrame frame( "getpgrp" );
		
		return CurrentProcess().GetPGID();
	}
	
	REGISTER_SYSTEM_CALL( getpgrp );
	
	static pid_t getpid()
	{
		SystemCallFrame frame( "getpid" );
		
		return CurrentProcess().GetPID();
	}
	
	REGISTER_SYSTEM_CALL( getpid );
	
	static pid_t getppid()
	{
		SystemCallFrame frame( "getppid" );
		
		return CurrentProcess().GetPPID();
	}
	
	REGISTER_SYSTEM_CALL( getppid );
	
	static off_t lseek( int fd, off_t offset, int whence )
	{
		SystemCallFrame frame( "lseek" );
		
		try
		{
			RegularFileHandle& fh = GetFileHandleWithCast< RegularFileHandle >( fd );
			
			return fh.Seek( offset, whence );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return frame.SetErrno( ESPIPE );
	}
	
	REGISTER_SYSTEM_CALL( lseek );
	
	static int mknod( const char* /*path*/, mode_t /*mode*/, dev_t /*dev*/ )
	{
		SystemCallFrame frame( "mknod" );
		
		return frame.SetErrno( EINVAL );
	}
	
	REGISTER_SYSTEM_CALL( mknod );
	
	static int pause()
	{
		SystemCallFrame frame( "pause" );
		
		CurrentProcess().Raise( SIGSTOP );  // Sleep, until...
		
		return frame.SetErrno( EINTR );
	}
	
	REGISTER_SYSTEM_CALL( pause );
	
	static int pipe( int filedes[ 2 ] )
	{
		SystemCallFrame frame( "pipe" );
		
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		int reader = LowestUnusedFileDescriptor( 3 );
		int writer = LowestUnusedFileDescriptor( reader + 1 );
		
		boost::shared_ptr< Conduit > conduit( new Conduit );
		
		boost::shared_ptr< IOHandle > pipeIn ( new PipeInHandle ( conduit ) );
		boost::shared_ptr< IOHandle > pipeOut( new PipeOutHandle( conduit ) );
		
		AssignFileDescriptor( reader, pipeOut );
		AssignFileDescriptor( writer, pipeIn );
		
		filedes[ 0 ] = reader;
		filedes[ 1 ] = writer;
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( pipe );
	
	static int peek( int fd, const char** buffer, size_t minBytes )
	{
		SystemCallFrame frame( "peek" );
		
		try
		{
			StreamHandle& stream = GetFileHandleWithCast< StreamHandle >( fd );
			
			const std::string& peekBuffer = stream.Peek( minBytes );
			
			if ( buffer != NULL )
			{
				*buffer = peekBuffer.c_str();
			}
			
			return peekBuffer.size();
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
	}
	
	//REGISTER_SYSTEM_CALL( peek );
	
	static ssize_t read( int fd, void* buf, size_t count )
	{
		SystemCallFrame frame( "read" );
		
		Breathe();
		
		try
		{
			StreamHandle& stream = GetFileHandleWithCast< StreamHandle >( fd );
			
			int get = stream.Read( reinterpret_cast< char* >( buf ),
			                       count );
			
			return get;
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
	}
	
	REGISTER_SYSTEM_CALL( read );
	
	static int setpgid( pid_t pid, pid_t pgid )
	{
		SystemCallFrame frame( "setpgid" );
		
		try
		{
			if ( pgid < 0 )
			{
				p7::throw_errno( EINVAL );
			}
			
			Process& current = CurrentProcess();
			
			Process& target( pid != 0 ? GetProcess( pid )
			                          : current );
			
			bool target_is_child = current.GetPID() == target.GetPPID();
			
			if ( current.GetPID() != target.GetPID()  &&  !target_is_child )
			{
				p7::throw_errno( ESRCH );  // target is not self or a child
			}
			
			if ( target_is_child  &&  target.GetLifeStage() != kProcessStarting )
			{
				p7::throw_errno( EACCES );  // child already execve'd
			}
			
			if ( current.GetSID() != target.GetSID() )
			{
				p7::throw_errno( EPERM );  // child in different session
			}
			
			if ( current.GetSID() == current.GetPID() )
			{
				p7::throw_errno( EPERM );  // target is a session leader
			}
			
			if ( pgid == 0 )
			{
				pgid = target.GetPID();
			}
			
			const boost::shared_ptr< Session >& session = target.GetProcessGroup()->GetSession();
			
			target.SetProcessGroup( GetProcessGroupInSession( pgid, session ) );
			
			return 0;
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
	}
	
	REGISTER_SYSTEM_CALL( setpgid );
	
	static boost::shared_ptr< Session > NewSession( pid_t sid )
	{
		return boost::shared_ptr< Session >( new Session( sid ) );
	}
	
	static pid_t setsid()
	{
		SystemCallFrame frame( "setsid" );
		
		try
		{
			Process& current( frame.Caller() );
			
			int pid = current.GetPID();
			
			// throws EPERM if pgid already exists
			current.SetProcessGroup( GetProcessGroupInSession( pid, NewSession( pid ) ) );
			
			return pid;
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
	}
	
	REGISTER_SYSTEM_CALL( setsid );
	
	static int truncate( const char* path, off_t length )
	{
		SystemCallFrame frame( "truncate" );
		
		try
		{
			FSTreePtr file = ResolvePathname( path, CurrentProcess().GetCWD() );
			
			file->SetEOF( length );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( truncate );
	
	static int ftruncate( int fd, off_t length )
	{
		SystemCallFrame frame( "ftruncate" );
		
		try
		{
			RegularFileHandle& fh = GetFileHandleWithCast< RegularFileHandle >( fd );
			
			fh.SetEOF( length );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( ftruncate );
	
	// ttypair() is analogous to socketpair(), and creates a pseudo-tty device.
	// File descriptors refering to the master and slave respectively are returned
	// in filedes.
	// I made this up too.
	static int ttypair( int fds[ 2 ] )
	{
		SystemCallFrame frame( "ttypair" );
		
		try
		{
			boost::shared_ptr< IOHandle > master, slave;
			
			GetNewPseudoTTYPair( master, slave );
			
			int master_fd = LowestUnusedFileDescriptor( 3 );
			int slave_fd  = LowestUnusedFileDescriptor( master_fd + 1 );
			
			AssignFileDescriptor( master_fd, master );
			AssignFileDescriptor( slave_fd,  slave  );
			
			fds[ 0 ] = master_fd;
			fds[ 1 ] = slave_fd;
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( ttypair );
	
	static ssize_t write( int fd, const void* buf, size_t count )
	{
		SystemCallFrame frame( "write" );
		
		Breathe();
		
		try
		{
			StreamHandle& stream = GetFileHandleWithCast< StreamHandle >( fd );
			
			const char* data = reinterpret_cast< const char* >( buf );
			
			int put = stream.Write( data, count );
			
			return put;
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
	}
	
	REGISTER_SYSTEM_CALL( write );
	
}

