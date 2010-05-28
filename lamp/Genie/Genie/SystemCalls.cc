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
#include "sys/uio.h"
#include "unistd.h"

// Debug
#include "debug/assert.hh"

// Nitrogen
#include "Nitrogen/OSStatus.hh"

// poseven
#include "poseven/types/errno_t.hh"

// OSErrno
#include "OSErrno/OSErrno.hh"

// Genie
#include "Genie/Faults.hh"
#include "Genie/FileDescriptors.hh"
#include "Genie/FS/ResolvePathname.hh"
#include "Genie/IO/Directory.hh"
#include "Genie/IO/Pipe.hh"
#include "Genie/IO/PseudoTTY.hh"
#include "Genie/IO/RegularFile.hh"
#include "Genie/IO/Terminal.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


#ifndef SIGSTKFLT
#define SIGSTKFLT  (-1)
#endif

#ifndef O_CLOEXEC
#define O_CLOEXEC  0
#endif

#ifndef DUP_DUP2
#define DUP_DUP2  (-1)
#endif


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	static p7::errno_t GetErrnoFromException()
	{
		try
		{
			throw;
		}
		catch ( const p7::errno_t& errnum )
		{
			return errnum;
		}
		catch ( const N::OSStatus& err )
		{
			return OSErrno::ErrnoFromOSStatus( err );
		}
		catch ( ... )
		{
		}
		
		return EINVAL;
	}
	
	
	SystemCallFrame::SystemCallFrame( const char* name ) : itsCaller( CurrentProcess() ),
	                                                       itsName  ( name ),
	                                                       itsErrno ( 0 )
	{
		itsCaller.EnterSystemCall( name );
		
		const size_t space = N::ThreadCurrentStackSpace( N::GetCurrentThread() );
		
		if ( space < 8192 )
		{
			DeliverFatalSignal( SIGSTKFLT );
		}
		
		Breathe();
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
	
	
	#pragma mark -
	#pragma mark ¥ unistd ¥
	
	static unsigned int alarm( unsigned int seconds )
	{
		SystemCallFrame frame( "alarm" );
		
		return frame.Caller().SetAlarm( seconds );
	}
	
	
	static int chdir( const char* pathname )
	{
		SystemCallFrame frame( "chdir" );
		
		try
		{
			if ( pathname == NULL )
			{
				return frame.SetErrno( EINVAL );
			}
			
			FSTreePtr newCWD = ResolvePathname( pathname, frame.Caller().GetCWD() );
			
			ResolveLinks_InPlace( newCWD );
			
			frame.Caller().ChangeDirectory( newCWD );
			
			return 0;
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
	}
	
	
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
	
	
	static int dup3( int oldfd, int newfd, int flags )
	{
		SystemCallFrame frame( "dup3" );
		
		const bool dup2_semantics = flags == DUP_DUP2;
		
		if ( dup2_semantics )
		{
			flags = 0;
		}
		
		if ( flags & ~O_CLOEXEC )
		{
			// Invalid flags
			return frame.SetErrno( EINVAL );
		}
		
		const bool close_on_exec = flags & O_CLOEXEC;
		
		try
		{
			DuplicateFileDescriptor( oldfd, newfd, close_on_exec );
			
			const bool dup2_semantics = flags == DUP_DUP2;
			
			if ( oldfd == newfd  &&  !dup2_semantics )
			{
				return frame.SetErrno( EINVAL );
			}
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return newfd;
	}
	
	
	static void _exit( int status )
	{
		// The frame doesn't get destructed, but we compensate.
		// ResumeAfterFork() calls Resume() and LeaveSystemCall().
		SystemCallFrame frame( "_exit" );
		
		Process& current( frame.Caller() );
		
		current.Exit( status );  // doesn't return unless forked
		
		ASSERT( current.Forked() );
		
		GetProcess( current.GetPPID() ).ResumeAfterFork();  // doesn't return
	}
	
	
	static pid_t getpgid( pid_t pid )
	{
		SystemCallFrame frame( "getpgid" );
		
		try
		{
			Process& proc = pid == 0 ? frame.Caller()
			                         : GetProcess( pid );
			
			return proc.GetPGID();
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
	}
	
	
	static pid_t getpid()
	{
		SystemCallFrame frame( "getpid" );
		
		return frame.Caller().GetPID();
	}
	
	
	static pid_t getppid()
	{
		SystemCallFrame frame( "getppid" );
		
		return frame.Caller().GetPPID();
	}
	
	
	static pid_t getsid( pid_t pid )
	{
		SystemCallFrame frame( "getsid" );
		
		try
		{
			Process& proc = pid == 0 ? frame.Caller()
			                         : GetProcess( pid );
			
			return proc.GetSID();
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
	}
	
	
	static off_t lseek( int fd, off_t offset, int whence )
	{
		SystemCallFrame frame( "lseek" );
		
		try
		{
			if ( RegularFileHandle* fh = IOHandle_Cast< RegularFileHandle >( GetFileHandle( fd ).get() ) )
			{
				return fh->Seek( offset, whence );
			}
			
			if ( DirHandle* h = IOHandle_Cast< DirHandle >( GetFileHandle( fd ).get() ) )
			{
				return h->Seek( offset, whence );
			}
			
			// downcast failed
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return frame.SetErrno( ESPIPE );
	}
	
	
	static int mknod( const char* /*path*/, mode_t /*mode*/, dev_t /*dev*/ )
	{
		SystemCallFrame frame( "mknod" );
		
		return frame.SetErrno( EINVAL );
	}
	
	
	static int pause()
	{
		SystemCallFrame frame( "pause" );
		
		frame.Caller().Raise( SIGSTOP );  // Sleep, until...
		
		return frame.SetErrno( EINTR );
	}
	
	
	static inline boost::shared_ptr< IOHandle >
	//
	NewPipeReader( const boost::intrusive_ptr< Conduit >& conduit, bool nonblocking )
	{
		return seize_ptr( new PipeOutHandle( conduit, nonblocking ) );
	}
	
	static inline boost::shared_ptr< IOHandle >
	//
	NewPipeWriter( const boost::intrusive_ptr< Conduit >& conduit, bool nonblocking )
	{
		return seize_ptr( new PipeInHandle( conduit, nonblocking ) );
	}
	
	static int pipe2( int pipefd[ 2 ], int flags )
	{
		SystemCallFrame frame( "pipe2" );
		
		try
		{
			const bool close_on_exec = flags & O_CLOEXEC;
			const bool nonblocking   = flags & O_NONBLOCK;
			
			FileDescriptorMap& files = frame.Caller().FileDescriptors();
			
			int reader = LowestUnusedFileDescriptor( 3 );
			int writer = LowestUnusedFileDescriptor( reader + 1 );
			
			boost::intrusive_ptr< Conduit > conduit( new Conduit );
			
			boost::shared_ptr< IOHandle > pipeIn ( NewPipeWriter( conduit, nonblocking ) );
			boost::shared_ptr< IOHandle > pipeOut( NewPipeReader( conduit, nonblocking ) );
			
			AssignFileDescriptor( reader, pipeOut, close_on_exec );
			AssignFileDescriptor( writer, pipeIn,  close_on_exec );
			
			pipefd[ 0 ] = reader;
			pipefd[ 1 ] = writer;
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	
	static int peek( int fd, const char** buffer, size_t minBytes )
	{
		SystemCallFrame frame( "peek" );
		
		try
		{
			StreamHandle& stream = GetFileHandleWithCast< StreamHandle >( fd );
			
			if ( const plus::string* peekBuffer = stream.Peek( minBytes ) )
			{
				if ( buffer != NULL )
				{
					*buffer = peekBuffer->c_str();
				}
				
				return peekBuffer->size();
			}
			
			return 0;
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
	}
	
	
	static ssize_t pread( int fd, void* buf, size_t count, off_t offset )
	{
		SystemCallFrame frame( "pread" );
		
		if ( offset < 0 )
		{
			return frame.SetErrno( EINVAL );
		}
		
		try
		{
			RegularFileHandle& file = GetFileHandleWithCast< RegularFileHandle >( fd, ESPIPE );
			
			int get = file.Positioned_Read( reinterpret_cast< char* >( buf ),
			                                count,
			                                offset );
			
			return get;
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
	}
	
	static ssize_t read( int fd, void* buf, size_t count )
	{
		SystemCallFrame frame( "read" );
		
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
	
	
	static int setpgid( pid_t pid, pid_t pgid )
	{
		SystemCallFrame frame( "setpgid" );
		
		try
		{
			if ( pgid < 0 )
			{
				p7::throw_errno( EINVAL );
			}
			
			Process& current = frame.Caller();
			
			Process& target( pid != 0 ? GetProcess( pid )
			                          : current );
			
			bool target_is_self = pid == 0  ||  target.GetPID() == current.GetPID();
			
			if ( target_is_self )
			{
				// A session-leading child is in a different session, which we test for
				
				if ( target.GetSID() == target.GetPID() )
				{
					p7::throw_errno( EPERM );  // target is a session leader
				}
			}
			else
			{
				bool target_is_child = current.GetPID() == target.GetPPID();
				
				if ( !target_is_child )
				{
					p7::throw_errno( ESRCH );  // target is not self or a child
				}
				
				if ( target.GetLifeStage() != kProcessStarting )
				{
					p7::throw_errno( EACCES );  // child already execve'd
				}
				
				if ( current.GetSID() != target.GetSID() )
				{
					p7::throw_errno( EPERM );  // child in different session
				}
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
	
	
	static int truncate( const char* path, off_t length )
	{
		SystemCallFrame frame( "truncate" );
		
		try
		{
			FSTreePtr file = ResolvePathname( path, frame.Caller().GetCWD() );
			
			ResolveLinks_InPlace( file );
			
			file->SetEOF( length );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	
	static int ftruncate( int fd, off_t length )
	{
		SystemCallFrame frame( "ftruncate" );
		
		try
		{
			IOHandle* h = GetFileHandle( fd ).get();
			
			typedef RegularFileHandle FileHandle;
			
			if ( FileHandle* fh = IOHandle_Cast< FileHandle >( h ) )
			{
				fh->SetEOF( length );
			}
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	
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
	
	
	static ssize_t pwrite( int fd, const void* buf, size_t count, off_t offset )
	{
		SystemCallFrame frame( "pwrite" );
		
		if ( offset < 0 )
		{
			return frame.SetErrno( EINVAL );
		}
		
		try
		{
			RegularFileHandle& file = GetFileHandleWithCast< RegularFileHandle >( fd, ESPIPE );
			
			const char* buffer = reinterpret_cast< const char* >( buf );
			
			const ssize_t put = file.Positioned_Write( buffer, count, offset );
			
			return put;
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
	}
	
	static ssize_t write( int fd, const void* buf, size_t count )
	{
		SystemCallFrame frame( "write" );
		
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
	
	static ssize_t writev( int fd, const struct iovec *iov, int n_iov )
	{
		SystemCallFrame frame( "writev" );
		
		bool valid = n_iov > 0;
		
		if ( valid )
		{
		#ifdef IOV_MAX
			
			valid = valid  &&  n_iov <= IOV_MAX;
			
		#endif
			
			if ( valid )
			{
				ssize_t n_bytes = 0;
				
				for ( int i = 0;  i < n_iov;  ++i )
				{
					n_bytes += iov[ i ].iov_len;
					
					if ( n_bytes < 0 )
					{
						valid = false;  // ssize_t overflow
						
						break;
					}
				}
				
			}
		}
		
		if ( !valid )
		{
			return frame.SetErrno( EINVAL );
		}
		
		try
		{
			StreamHandle& stream = GetFileHandleWithCast< StreamHandle >( fd );
			
			ssize_t result = 0;
			
			for ( int i = 0;  i < n_iov;  ++i )
			{
				const char* buffer = (char*) iov[ i ].iov_base;
				
				const size_t length = iov[ i ].iov_len;
				
				const ssize_t written = stream.Write( buffer, length );
				
				result += written;
				
				if ( written != length )
				{
					break;
				}
			}
			
			return result;
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( InitProc  );
	
	REGISTER_SYSTEM_CALL( alarm     );
	REGISTER_SYSTEM_CALL( chdir     );
	REGISTER_SYSTEM_CALL( close     );
	REGISTER_SYSTEM_CALL( dup3      );
	REGISTER_SYSTEM_CALL( _exit     );
	REGISTER_SYSTEM_CALL( getpgid   );
	REGISTER_SYSTEM_CALL( getpid    );
	REGISTER_SYSTEM_CALL( getppid   );
	REGISTER_SYSTEM_CALL( getsid    );
	REGISTER_SYSTEM_CALL( lseek     );
	REGISTER_SYSTEM_CALL( mknod     );
	REGISTER_SYSTEM_CALL( pause     );
	REGISTER_SYSTEM_CALL( pipe2     );
	//REGISTER_SYSTEM_CALL( peek );
	REGISTER_SYSTEM_CALL( pread     );
	REGISTER_SYSTEM_CALL( read      );
	REGISTER_SYSTEM_CALL( setpgid   );
	REGISTER_SYSTEM_CALL( setsid    );
	REGISTER_SYSTEM_CALL( truncate  );
	REGISTER_SYSTEM_CALL( ftruncate );
	REGISTER_SYSTEM_CALL( ttypair   );
	REGISTER_SYSTEM_CALL( write     );
	REGISTER_SYSTEM_CALL( writev    );
	REGISTER_SYSTEM_CALL( pwrite    );
	
	#pragma force_active reset
	
}

