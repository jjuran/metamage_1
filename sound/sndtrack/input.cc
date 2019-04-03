/*
	input.cc
	--------
*/

#include "input.hh"

// POSIX
#include <unistd.h>


ssize_t read_all( int fd, char* p, size_t n )
{
	const size_t m = n;
	
	ssize_t n_read = 1;
	
	while ( n > 0  &&  (n_read = read( fd, p, n )) > 0 )
	{
		p += n_read;
		n -= n_read;
	}
	
	return + n == 0 ? m          // success, return original n
	       : n_read ? n_read     // read() error, return -1
	       : m == n ? 0          // EOF at zero bytes, return 0
	       :          ~(m - n);  // EOF later, return total read as value < -1
}
