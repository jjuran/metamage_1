/*	===========
 *	realpath.cc
 *	===========
 */

// Standard C
#include <setjmp.h>

// POSIX
#include <unistd.h>

// Historical
#include <alloca.h>

// Iota
#include "iota/strings.hh"

// more-posix
#include "more/perror.hh"


int main( int argc, char const *const argv[] )
{
	// Check for correct number of args
	if ( argc != 2 )
	{
		(void) write( STDERR_FILENO, STR_LEN( "Usage: realpath <file>\n" ) );
		
		return 1;
	}
	
	const char *const pathname = argv[1];
	
	volatile size_t buffer_size = 128;  // reasonable starting point
	
	sigjmp_buf state;
	
	// Save the CPU state
	sigsetjmp( state, false );
	
	{
		// Allocate a buffer on the stack
		char *buffer = (char*) alloca( buffer_size );
		
		ssize_t actual_size = realpath_k( pathname, buffer, buffer_size - 1 );
		
		if ( actual_size == -1 )
		{
			more::perror( "realpath", pathname );
			
			return 1;
		}
		
		if ( actual_size < 0 )
		{
			// Buffer too small; increase and try again
			
			// result of realpath_k() is the bitwise negation of the actual size
			// or the additive inverse of the corresponding output size
			
			buffer_size = -actual_size;
			
			// This pops our previous buffer off the stack
			siglongjmp( state, 1 );
		}
		
		buffer[ actual_size++ ] = '\n';
		
		(void) write( STDOUT_FILENO, buffer, actual_size );
	}
	
	return 0;
}

