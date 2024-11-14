/*
	read_all.hh
	-----------
*/

#ifndef POSIX_READALL_HH
#define POSIX_READALL_HH

// POSIX
#include <sys/types.h>


ssize_t read_all( int fd, char* p, size_t n );

inline
ssize_t read_all( int fd, void* p, size_t n )
{
	return read_all( fd, (char*) p, n );
}

#endif
