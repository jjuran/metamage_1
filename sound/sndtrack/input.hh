/*
	input.hh
	--------
*/

#ifndef INPUT_HH
#define INPUT_HH

// POSIX
#include <sys/types.h>

ssize_t read_all( int fd, char* p, size_t n );

inline
ssize_t read_all( int fd, void* p, size_t n )
{
	return read_all( fd, (char*) p, n );
}

#endif
