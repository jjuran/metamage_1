/*
	load_file.cc
	------------
*/

#include "frend/load_file.hh"

// POSIX
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

// Standard C
#include <errno.h>

// Extended API Set, Part 2
#include "extended-api-set/part-2.h"

// plus
#include "plus/string.hh"


namespace frend
{

static
plus::string load( int fd )
{
	plus::string s;
	
	char* p = NULL;
	
	struct stat st;
	
	size_t n;
	
	int nok = fstat( fd, &st )                                ||
	          (! S_ISREG( st.st_mode )  &&
	           (errno = ESPIPE))                              ||
	          ! (n = st.st_size,p = s.reset_nothrow( n ))     ||
	          (read( fd, p, n ) != n  &&
	           (errno = EAGAIN));
	
	if ( nok )
	{
		return plus::string::null;
	}
	
	return s;
}

plus::string load_file( int dirfd, const char* path )
{
	int fd = openat( dirfd, path, O_RDONLY | O_NONBLOCK );
	
	if ( fd < 0 )
	{
		return plus::string::null;
	}
	
	plus::string data = load( fd );
	
	close( fd );
	
	return data;
}

}
