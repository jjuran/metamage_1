/*
	pump.cc
	-------
	
	Joshua Juran
*/

#include "pump.h"

// Standard C++
#include <algorithm>

// POSIX
#include <errno.h>
#include <unistd.h>


#ifndef __LAMP__

ssize_t pump( int fd_in, off_t* off_in, int fd_out, off_t* off_out, size_t count, unsigned flags )
{
	const std::size_t buffer_size = 4096;
	
	char buffer[ buffer_size ];
	
	if ( off_in != NULL )
	{
		if ( lseek( fd_in, *off_in, 0 ) == -1 )
		{
			return -1;
		}
	}
	
	if ( off_out != NULL )
	{
		if ( lseek( fd_out, *off_out, 0 ) == -1 )
		{
			return -1;
		}
	}
	
	ssize_t bytes_pumped = 0;
	
	while ( int bytes_read = read( fd_in, buffer, count ? std::min( count - bytes_pumped, buffer_size ) : buffer_size ) )
	{
		if ( bytes_read == -1 )
		{
			return bytes_pumped == 0 ? -1 : bytes_pumped;
		}
		
		int bytes_written = write( fd_out, buffer, bytes_read );
		
		if ( bytes_written == -1 )
		{
			return -1;
		}
		
		if ( bytes_written != bytes_read )
		{
			errno = EIO;
			return -1;
		}
		
		bytes_pumped += bytes_written;
	}
	
	if ( off_in != NULL )
	{
		lseek( fd_in, *off_in, 0 );
		
		*off_in += bytes_pumped;
	}
	
	if ( off_out != NULL )
	{
		lseek( fd_out, *off_out, 0 );
		
		*off_out += bytes_pumped;
	}
	
	return bytes_pumped;
}

#endif

