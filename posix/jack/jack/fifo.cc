/*
	fifo.cc
	-------
*/

#include "jack/fifo.hh"

// POSIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

// Standard C
#include <errno.h>


namespace jack
{
	
	int fifo_wait( const char* path )
	{
		int fifo_fd = open( path, O_WRONLY );
		
		if ( fifo_fd < 0 )
		{
			return fifo_fd;
		}
		
		return close( fifo_fd );
	}
	
	int fifo_ready( const char* path, mode_t mode )
	{
		if ( mode != 0 )
		{
			int nok = mkfifo( path, mode );
			
			if ( nok < 0  &&  errno != EEXIST )
			{
				return nok;
			}
		}
		
		return open( path, O_RDONLY | O_NONBLOCK );
	}
	
}
