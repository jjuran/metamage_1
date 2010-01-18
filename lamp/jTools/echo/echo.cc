/*	=======
 *	echo.cc
 *	=======
 */

// Standard C
#include <string.h>

// POSIX
#include <unistd.h>
#include <sys/uio.h>

// Historical
#include <alloca.h>


#pragma exceptions off


int main( int argc, char **argv )
{
	struct iovec end = { (void*) "\n", 1 };
	
	struct iovec *v = &end;
	
	size_t n_strings = 1;
	
	if ( const int n_args = argc - 1 )
	{
		n_strings = n_args * 2;
		
		const size_t storage_size = n_strings * sizeof (struct iovec);
		
		v = (struct iovec *) alloca( storage_size );
		
		const char *const *args = argv + 1;
		
		struct iovec space = { (void*) " ", 1 };
		
		for ( int i = 0;  i < n_args;  ++i )
		{
			v[ i * 2 ].iov_base = (void*) args[ i ];
			v[ i * 2 ].iov_len  = strlen( args[ i ] );
			
			v[ i * 2 + 1 ] = space;
		}
		
		v[ n_strings - 1 ].iov_base = end.iov_base;
	}
	
	const int n_written = writev( STDOUT_FILENO, v, n_strings );
	
	return n_written < 0 ? 1 : 0;
}

