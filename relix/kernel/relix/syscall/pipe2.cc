/*
	pipe2.cc
	--------
*/

#include "relix/syscall/pipe2.hh"

// POSIX
#include <fcntl.h>

// vfs
#include "vfs/filehandle.hh"

// relix
#include "relix/api/assign_fd.hh"
#include "relix/api/errno.hh"
#include "relix/api/first_free_fd.hh"
#include "relix/fs/pipe.hh"


#ifndef O_CLOEXEC
#define O_CLOEXEC  0
#endif


namespace relix
{
	
	int pipe2( int pipefd[ 2 ], int flags )
	{
		try
		{
			const bool close_on_exec = flags & O_CLOEXEC;
			
			int reader_fd = first_free_fd( 3 );
			int writer_fd = first_free_fd( reader_fd + 1 );
			
			pipe_ends ends = new_pipe( flags & O_NONBLOCK );
			
			assign_fd( reader_fd, *ends.reader, close_on_exec );
			assign_fd( writer_fd, *ends.writer, close_on_exec );
			
			pipefd[ 0 ] = reader_fd;
			pipefd[ 1 ] = writer_fd;
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
}

