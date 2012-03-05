/*
	beep.c
	------
*/

// POSIX
#include <unistd.h>


int main( void )
{
	const char c = 0x07;
	
	write( STDOUT_FILENO, &c, sizeof c );
	
	return 0;
}

