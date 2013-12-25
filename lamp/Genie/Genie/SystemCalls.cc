/*	==============
 *	SystemCalls.cc
 *	==============
 */

// Standard C
#include <errno.h>

// POSIX
#include <fcntl.h>
#include "sys/uio.h"

// Debug
#include "debug/assert.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle/functions/seek.hh"
#include "vfs/filehandle/primitives/read.hh"
#include "vfs/filehandle/primitives/write.hh"

// relix-kernel
#include "relix/api/assign_fd.hh"
#include "relix/api/current_process.hh"
#include "relix/api/first_free_fd.hh"
#include "relix/api/get_fd_handle.hh"
#include "relix/signal/caught_signal.hh"
#include "relix/syscall/alarm.hh"
#include "relix/syscall/chdir.hh"
#include "relix/syscall/close.hh"
#include "relix/syscall/dup3.hh"
#include "relix/syscall/ftruncate.hh"
#include "relix/syscall/getpgid.hh"
#include "relix/syscall/getpid.hh"
#include "relix/syscall/getppid.hh"
#include "relix/syscall/getsid.hh"
#include "relix/syscall/gettid.hh"
#include "relix/syscall/pread.hh"
#include "relix/syscall/pwrite.hh"
#include "relix/syscall/truncate.hh"
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


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	using relix::alarm;
	using relix::chdir;
	using relix::close;
	using relix::dup3;
	
	
	static void _exit( int status )
	{
		// We don't return to the dispatcher, but Terminate() compensates.
		
		Process& current = current_process();
		
		current.Exit( status );  // doesn't return
		
		ASSERT( false && "Process::Exit() doesn't return" );
	}
	
	
	using relix::getpgid;
	using relix::getpid;
	using relix::getppid;
	using relix::getsid;
	using relix::gettid;
	
	
	static off_t lseek( int fd, off_t offset, int whence )
	{
		try
		{
			vfs::filehandle* file = &relix::get_fd_handle( fd );
			
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
			
			int reader_fd = relix::first_free_fd( 3 );
			int writer_fd = relix::first_free_fd( reader_fd + 1 );
			
			pipe_ends ends = new_pipe( flags & O_NONBLOCK );
			
			relix::assign_fd( reader_fd, *ends.reader, close_on_exec );
			relix::assign_fd( writer_fd, *ends.writer, close_on_exec );
			
			pipefd[ 0 ] = reader_fd;
			pipefd[ 1 ] = writer_fd;
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
	
	using relix::pread;
	
	static ssize_t read( int fd, void* buf, size_t count )
	{
		try
		{
			return read( relix::get_fd_handle( fd ), (char*) buf, count );
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
	
	
	using relix::truncate;
	using relix::ftruncate;
	using relix::pwrite;
	
	
	static ssize_t write( int fd, const void* buf, size_t count )
	{
		try
		{
			return write( relix::get_fd_handle( fd ), (const char*) buf, count );
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

