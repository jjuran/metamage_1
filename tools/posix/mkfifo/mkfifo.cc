/*
	mkfifo.cc
	---------
*/

// POSIX
#include <unistd.h>
#include <sys/stat.h>

// more-posix
#include "more/perror.hh"


#define PROGRAM  "mkfifo"

#define USAGE  "usage: " PROGRAM " path ...\n"

#define STR_LEN( s )  "" s, (sizeof s - 1)


#define CHECK_N( call )  check_n( call, #call )

static int exit_status = 0;

static inline int check_n( int result, const char* code )
{
	if ( result < 0 )
	{
		more::perror( PROGRAM " error", code );
		
		exit_status = 1;
	}
	
	return result;
}

static int usage()
{
	write( STDERR_FILENO, STR_LEN( USAGE ) );
	
	return 2;
}

int main( int argc, char** argv )
{
	if ( argc < 1 + 1 )
	{
		return usage();
	}
	
	for ( int i = 1;  i < argc;  ++i )
	{
		const char* path = argv[ i ];
		
		CHECK_N( mknod( path, S_IFIFO | 0666, 0 ) );
	}
	
	return exit_status;
}
