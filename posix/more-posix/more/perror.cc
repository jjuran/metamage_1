/*
	more/perror.cc
	--------------
	
	Copyright 2009 Joshua Juran
*/

#include "more/perror.hh"

// Standard C
#include <string.h>

// POSIX
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>


#pragma exceptions off


#define COUNT( v )  (sizeof v / sizeof *v)

#define COUNTED( v )  v, COUNT( v )

#define STR_LEN( s )  "" s, (sizeof s - 1)


namespace more
{

#define WRITE_IOVEC_TO_STDERR( iov )  write_iovec_to_stderr( COUNTED( iov ) )

static
void write_iovec_to_stderr( const iovec* iov, size_t n )
{
	ssize_t written = writev( STDERR_FILENO, iov, n );
	
	if ( written < 0 )
	{
		_exit( 13 );
	}
}

void perror( int errnum )
{
	const char* error = strerror( errnum );
	
	struct iovec iov[] =
	{
		{ (char*) error, strlen( error ) },
		{ (char*) STR_LEN( "\n" )        },
	};
	
	WRITE_IOVEC_TO_STDERR( iov );
}

void perror( const char* s, int errnum )
{
	const char* error = strerror( errnum );
	
	struct iovec iov[] =
	{
		{ (char*) s, strlen( s )         },
		{ (char*) STR_LEN( ": " )        },
		{ (char*) error, strlen( error ) },
		{ (char*) STR_LEN( "\n" )        },
	};
	
	if ( errnum == 0 )
	{
		iov[ 1 ].iov_len = 0;
		iov[ 2 ].iov_len = 0;
	}
	
	WRITE_IOVEC_TO_STDERR( iov );
}

void perror( const char* s1, const char* s2, int errnum )
{
	const char* error = strerror( errnum );
	
	struct iovec iov[] =
	{
		{ (char*) s1, strlen( s1 )       },
		{ (char*) STR_LEN( ": " )        },
		{ (char*) s2, strlen( s2 )       },
		{ (char*) STR_LEN( ": " )        },
		{ (char*) error, strlen( error ) },
		{ (char*) STR_LEN( "\n" )        },
	};
	
	if ( errnum == 0 )
	{
		iov[ 3 ].iov_len = 0;
		iov[ 4 ].iov_len = 0;
	}
	
	WRITE_IOVEC_TO_STDERR( iov );
}

void perror( const char* s1, const char* s2, const char* s3 )
{
	struct iovec iov[] =
	{
		{ (char*) s1, strlen( s1 ) },
		{ (char*) STR_LEN( ": " )  },
		{ (char*) s2, strlen( s2 ) },
		{ (char*) STR_LEN( ": " )  },
		{ (char*) s3, strlen( s3 ) },
		{ (char*) STR_LEN( "\n" )  },
	};
	
	WRITE_IOVEC_TO_STDERR( iov );
}

}
