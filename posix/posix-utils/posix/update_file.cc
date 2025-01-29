/*
	update_file.cc
	--------------
*/

#include "posix/update_file.hh"

// POSIX
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

// Standard C
#include <errno.h>
#include <stdlib.h>
#include <string.h>


#pragma exceptions off


namespace posix
{

int update_file( int fd, const void* current, unsigned size )
{
	struct stat st;
	
	int nok = fstat( fd, &st );
	
	if ( nok )
	{
		return -1;
	}
	
	if ( ! S_ISREG( st.st_mode ) )
	{
		errno = EINVAL;
		
		return -1;
	}
	
	if ( st.st_size == size )
	{
		char* previous = (char*) malloc( size );
		
		if ( previous == NULL )
		{
			return -1;
		}
		
		ssize_t n = pread( fd, previous, size, 0 );
		
		bool same = n == size  &&  memcmp( current, previous, n ) == 0;
		
		free( previous );
		
		if ( same )
		{
			return 0;
		}
	}
	
	ssize_t n = write( fd, current, size );
	
	if ( n == size )
	{
		return 0;
	}
	
	if ( n >= 0 )
	{
		errno = EIO;
	}
	
	return -1;
}

int update_file( const char* path, const void* current, unsigned size )
{
	int fd = open( path, O_RDWR | O_CREAT, 0666 );
	
	if ( fd < 0 )
	{
		return fd;
	}
	
	int nok = update_file( fd, current, size );
	
	int saved_errno = errno;
	
	close( fd );
	
	errno = saved_errno;
	
	return nok;
}

}
