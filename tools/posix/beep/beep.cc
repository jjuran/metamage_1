/*
	beep.cc
	-------
*/

// POSIX
#include <unistd.h>


static inline
bool wrote( int fd, const void* buffer, size_t n )
{
	return write( fd, buffer, n ) == n;
}

int main( void )
{
	const char c = 0x07;
	
	return wrote( STDOUT_FILENO, &c, sizeof c ) ? 0 : 13;
}
