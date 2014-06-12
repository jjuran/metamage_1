/*
	fifo-write.cc
	-------------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define PROGRAM  "fifo-write"

#define USAGE  "usage: " PROGRAM " path\n"

#define STR_LEN( s )  "" s, (sizeof s - 1)


#define CHECK_N( call )  check_n( call, #call )

static
int check_n( int result, const char* code )
{
	if ( result < 0 )
	{
		fprintf( stderr, PROGRAM " error: %s: %s\n", code, strerror( errno ) );
		
		exit( 1 );
	}
	
	return result;
}

static
int usage()
{
	write( STDERR_FILENO, STR_LEN( USAGE ) );
	
	return 2;
}

int main( int argc, char** argv )
{
	if ( argc != 1 + 1 )
	{
		return usage();
	}
	
	const char* path = argv[ 1 ];
	
	int fifo_fd = CHECK_N( open( path, O_WRONLY ) );
	
	char buffer[ 2048 ] = { 0 };
	
	while ( true )
	{
		CHECK_N( write( STDOUT_FILENO, STR_LEN( "> " ) ) );
		
		CHECK_N( read( STDIN_FILENO, buffer, 1 ) );
		
		CHECK_N( write( fifo_fd, buffer, sizeof buffer ) );
	}
	
	return 0;
}
