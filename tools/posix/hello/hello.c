/*
	hello.c
	-------
*/

// POSIX
#include <unistd.h>


#define HELLO  "Hello world\n"

int main()
{
	write( STDOUT_FILENO, HELLO, sizeof HELLO - 1 );
	
	return 0;
}

