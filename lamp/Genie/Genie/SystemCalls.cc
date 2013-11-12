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

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/primitives/seteof.hh"
#include "vfs/filehandle/functions/seek.hh"
#include "vfs/filehandle/primitives/pread.hh"
#include "vfs/filehandle/primitives/pwrite.hh"
#include "vfs/filehandle/primitives/seteof.hh"
#include "vfs/functions/file-tests.hh"
#include "vfs/functions/resolve_links_in_place.hh"
#include "vfs/functions/resolve_pathname.hh"

// relix-kernel
#include "relix/api/current_process.hh"
#include "relix/api/getcwd.hh"
#include "relix/signal/caught_signal.hh"
#include "relix/syscall/alarm.hh"
#include "relix/syscall/getpid.hh"
#include "relix/syscall/getppid.hh"
#include "relix/syscall/gettid.hh"
#include "relix/task/process.hh"
#include "relix/task/process_group.hh"
#include "relix/task/session.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/Faults.hh"
#include "Genie/FileDescriptors.hh"
#include "Genie/IO/Directory.hh"
#include "Genie/IO/Pipe.hh"
#include "Genie/IO/Stream.hh"
#include "Genie/IO/Terminal.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"


#ifndef O_CLOEXEC
#define O_CLOEXEC  0
#endif

#ifndef DUP_DUP2
#define DUP_DUP2  (-1)
#endif


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	using relix::alarm;
	
	
	static int chdir( const char* pathname )
	{
		try
		{
			if ( pathname == NULL )
			{
				return set_errno( EINVAL );
			}
			
			vfs::node_ptr newCWD = resolve_pathname( pathname, *relix::getcwd() );
			
			vfs::resolve_links_in_place( newCWD );
			
			if ( !is_directory( *newCWD ) )
			{
				return set_errno( exists( *newCWD ) ? ENOTDIR : ENOENT );
			}
			
			current_process().ChangeDirectory( *newCWD );
			
			return 0;
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
	}
	
	
	static int close( int fd )
	{
		try
		{
			CloseFileDescriptor( fd );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
	
	static int dup3( int oldfd, int newfd, int flags )
	{
		const bool dup2_semantics = flags == DUP_DUP2;
		
		if ( dup2_semantics )
		{
			flags = 0;
		}
		
		if ( flags & ~O_CLOEXEC )
		{
			// Invalid flags
			return set_errno( EINVAL );
		}
		
		const bool close_on_exec = flags & O_CLOEXEC;
		
		try
		{
			DuplicateFileDescriptor( oldfd, newfd, close_on_exec );
			
			if ( oldfd == newfd  &&  !dup2_semantics )
			{
				return set_errno( EINVAL );
			}
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return newfd;
	}
	
	
	static void _exit( int status )
	{
		// We don't return to the dispatcher, but Terminate() compensates.
		
		Process& current = current_process();
		
		current.Exit( status );  // doesn't return
		
		ASSERT( false && "Process::Exit() doesn't return" );
	}
	
	
	static pid_t getpgid( pid_t pid )
	{
		try
		{
			Process& proc = pid == 0 ? current_process()
			                         : GetProcess( pid );
			
			return proc.GetPGID();
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
	}
	
	
	using relix::getpid;
	using relix::getppid;
	using relix::gettid;
	
	
	static pid_t getsid( pid_t pid )
	{
		try
		{
			Process& proc = pid == 0 ? current_process()
			                         : GetProcess( pid );
			
			return proc.GetSID();
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
	}
	
	
	static off_t lseek( int fd, off_t offset, int whence )
	{
		try
		{
			vfs::filehandle* file = &get_filehandle( fd );
			
			if ( DirHandle* h = IOHandle_Cast< DirHandle >( file ) )
			{
				return h->Seek( offset, whence );
			}
			
			return seek( *file, offset, whence );
			
			// downcast failed
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return set_errno( ESPIPE );
	}
	
	
	static int pause()
	{
		try
		{
			Process& current = current_process();
			
			while ( true )
			{
				current.Stop();
				
				current.HandlePendingSignals( true );
			}
		}
		catch ( ... )
		{
			(void) set_errno_from_exception();
		}
		
		relix::prevent_syscall_restart();
		
		return -1;  // EINTR
	}
	
	
	static int pipe2( int pipefd[ 2 ], int flags )
	{
		try
		{
			const bool close_on_exec = flags & O_CLOEXEC;
			
			int reader_fd = LowestUnusedFileDescriptor( 3 );
			int writer_fd = LowestUnusedFileDescriptor( reader_fd + 1 );
			
			pipe_ends ends = new_pipe( flags & O_NONBLOCK );
			
			assign_file_descriptor( reader_fd, *ends.reader, close_on_exec );
			assign_file_descriptor( writer_fd, *ends.writer, close_on_exec );
			
			pipefd[ 0 ] = reader_fd;
			pipefd[ 1 ] = writer_fd;
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
	
	static int peek( int fd, const char** buffer, size_t minBytes )
	{
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
			return set_errno_from_exception();
		}
	}
	
	
	static ssize_t pread( int fd, void* buf, size_t count, off_t offset )
	{
		if ( offset < 0 )
		{
			return set_errno( EINVAL );
		}
		
		try
		{
			ssize_t get = pread( get_filehandle( fd ),
			                     (char*) buf,
			                     count,
			                     offset );
			
			return get;
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
	}
	
	static ssize_t read( int fd, void* buf, size_t count )
	{
		try
		{
			StreamHandle& stream = GetFileHandleWithCast< StreamHandle >( fd );
			
			int get = stream.Read( reinterpret_cast< char* >( buf ),
			                       count );
			
			return get;
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
	}
	
	
	static int setpgid( pid_t pid, pid_t pgid )
	{
		try
		{
			if ( pgid < 0 )
			{
				p7::throw_errno( EINVAL );
			}
			
			Process& current = current_process();
			
			Process& target( pid != 0 ? GetProcess( pid )
			                          : current );
			
			relix::process& target_process = target.get_process();
			
			relix::session& session = target_process.get_process_group().get_session();
			
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
			
			target_process.set_process_group( *GetProcessGroupInSession( pgid, session ) );
			
			return 0;
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
	}
	
	
	static pid_t setsid()
	{
		try
		{
			relix::process& current = relix::current_process();
			
			const int pid = current.id();
			
			// throws EPERM if pgid already exists
			current.set_process_group( *GetProcessGroupInSession( pid, *NewSession( pid ) ) );
			
			return pid;
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
	}
	
	
	static int truncate( const char* path, off_t length )
	{
		try
		{
			vfs::node_ptr file = resolve_pathname( path, *relix::getcwd() );
			
			vfs::resolve_links_in_place( file );
			
			seteof( *file, length );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
	
	static int ftruncate( int fd, off_t length )
	{
		try
		{
			seteof( get_filehandle( fd ), length );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
	
	static ssize_t pwrite( int fd, const void* buf, size_t count, off_t offset )
	{
		if ( offset < 0 )
		{
			return set_errno( EINVAL );
		}
		
		try
		{
			vfs::filehandle& file = get_filehandle( fd );
			
			const char* buffer = reinterpret_cast< const char* >( buf );
			
			const ssize_t put = pwrite( file, buffer, count, offset );
			
			return put;
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
	}
	
	static ssize_t write( int fd, const void* buf, size_t count )
	{
		try
		{
			StreamHandle& stream = GetFileHandleWithCast< StreamHandle >( fd );
			
			const char* data = reinterpret_cast< const char* >( buf );
			
			int put = stream.Write( data, count );
			
			return put;
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
	}
	
	static ssize_t writev( int fd, const struct iovec *iov, int n_iov )
	{
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
			return set_errno( EINVAL );
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
			return set_errno_from_exception();
		}
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( alarm     );
	REGISTER_SYSTEM_CALL( chdir     );
	REGISTER_SYSTEM_CALL( close     );
	REGISTER_SYSTEM_CALL( dup3      );
	REGISTER_SYSTEM_CALL( _exit     );
	REGISTER_SYSTEM_CALL( getpgid   );
	REGISTER_SYSTEM_CALL( getpid    );
	REGISTER_SYSTEM_CALL( getppid   );
	REGISTER_SYSTEM_CALL( getsid    );
	REGISTER_SYSTEM_CALL( gettid    );
	REGISTER_SYSTEM_CALL( lseek     );
	REGISTER_SYSTEM_CALL( pause     );
	REGISTER_SYSTEM_CALL( pipe2     );
	//REGISTER_SYSTEM_CALL( peek );
	REGISTER_SYSTEM_CALL( pread     );
	REGISTER_SYSTEM_CALL( read      );
	REGISTER_SYSTEM_CALL( setpgid   );
	REGISTER_SYSTEM_CALL( setsid    );
	REGISTER_SYSTEM_CALL( truncate  );
	REGISTER_SYSTEM_CALL( ftruncate );
	REGISTER_SYSTEM_CALL( write     );
	REGISTER_SYSTEM_CALL( writev    );
	REGISTER_SYSTEM_CALL( pwrite    );
	
	#pragma force_active reset
	
}

