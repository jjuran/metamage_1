/*
	recording.cc
	------------
*/

#include "recording.hh"

// POSIX
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

// Standard C
#include <errno.h>


#define RECORDING_FAILED  "\n" "*** Recording failed, aborting ***" "\n\n"

#define STR_LEN( s )  "" s, (sizeof s - 1)


namespace v68k   {
namespace record {

int recording_fd = -1;


int append( const void* data, unsigned size )
{
	if ( ! recording() )
	{
		return EBADF;
	}
	
	ssize_t n_written = write( recording_fd, data, size );
	
	if ( n_written < 0 )
	{
		int errnum = errno;
		
		close( recording_fd );
		
		recording_fd = -1;
		
		write( STDERR_FILENO, STR_LEN( RECORDING_FAILED ) );
		
		return errnum;
	}
	
	return 0;
}

int appendv( const iovec* iov, unsigned n_iov )
{
	if ( ! recording() )
	{
		return EBADF;
	}
	
	ssize_t n_written = writev( recording_fd, iov, n_iov );
	
	if ( n_written < 0 )
	{
		int errnum = errno;
		
		close( recording_fd );
		
		recording_fd = -1;
		
		write( STDERR_FILENO, STR_LEN( RECORDING_FAILED ) );
		
		return errnum;
	}
	
	return 0;
}

}  // namespace record
}  // namespace v68k
