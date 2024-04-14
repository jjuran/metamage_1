/*
	pause.c
	-------
*/

// POSIX
#include <unistd.h>


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define PRINT( msg )  write( STDOUT_FILENO, STR_LEN( msg "\n" ) )


int main( int argc, char** argv )
{
	ssize_t bytes = PRINT( "Hit return to continue" );
	
	char c;
	
	bytes = read( STDIN_FILENO, &c, sizeof c );
	
	return bytes ? 0 : 1;
}
