/*
	lib-static.cc
	-------------
	
	This tool is an abstraction for building a static library.  By
	making it a compiled program, we can use preprocessor macros to
	inform potential customizations.
	
	In order for this program to be usable as build infrastructure,
	it's vitally important that it be buildable without the use of
	static libraries in source-only contexts.  (This concern doesn't
	apply to MacRelix, since it ships with necessary build tools.)
*/

// POSIX
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

// Standard C
#include <errno.h>
#include <string.h>


#define PROGRAM  "lib-static"

#define USAGE "Usage: lib-static -o output-path input-path...\n"

#define COUNT( v )  (sizeof v / sizeof *v)

#define COUNTED( v )  v, COUNT( v )

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define AR  "ar"
#define RCS "rcs"

#ifdef __GNUC__
#if __GNUC__ < 3
/*
	The toolchain in the Mac OS X 10.1 SDK doesn't like `ar rcs`,
	so we need to use `ar rcu` instead.  Since AvailabilityMacros.h
	doesn't exist in the 10.1 SDK, we can't include it to test for
	MAC_OS_X_VERSION_10_2, so instead we use the GCC version as a
	distinguishing mark:  In 10.1 it's 2, whereas in 10.2 it's 3.
*/
#undef RCS
#define RCS "rcu"
#endif
#endif


static inline
int usage()
{
	ssize_t written = write( STDERR_FILENO, STR_LEN( USAGE ) );
	
	return written < 0 ? 13 : 1;
}

#define WRITE_IOVEC_TO_STDERR( iov )  write_iovec_to_stderr( COUNTED( iov ) )

static inline
void write_iovec_to_stderr( const iovec* iov, size_t n )
{
	ssize_t written = writev( STDERR_FILENO, iov, n );
	
	if ( written < 0 )
	{
		_exit( 13 );
	}
}

static inline
void report_error()
{
	const char* error = strerror( errno );
	
	struct iovec iov[] =
	{
		{ (char*) STR_LEN( PROGRAM ": " AR ": " ) },
		{ (char*) error, strlen( error )          },
		{ (char*) STR_LEN( "\n" )                 },
	};
	
	WRITE_IOVEC_TO_STDERR( iov );
}

int main( int argc, char** argv )
{
	if ( argc < 3  ||  strcmp( argv[ 1 ], "-o" ) != 0 )
	{
		return usage();
	}
	
	argv[ 0 ] = (char*) AR;
	argv[ 1 ] = (char*) RCS;
	
	const char* output_path = argv[ 2 ];
	
#ifndef __RELIX__
	
	/*
		Delete the output file each time so we don't get cruft
		left over from last time.
		
		The Metrowerks linker doesn't have this problem, though,
		so we don't need to delete the file in MacRelix.
	*/
	
	unlink( output_path );
	
#endif
	
	execvp( argv[ 0 ], argv );
	
	int saved_errno = errno;
	
	report_error();
	
	return saved_errno == ENOENT ? 127 : 126;
}
