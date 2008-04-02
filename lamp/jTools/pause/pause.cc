/*	========
 *	pause.cc
 *	========
 */

// POSIX
#include <unistd.h>

// Iota
#include "iota/strings.hh"


int main( int argc, char const *const argv[] )
{
	(void) write( STDOUT_FILENO, STR_LEN( "Hit return to continue\n" ) );
	
	char c;
	
	ssize_t bytes = read( STDIN_FILENO, &c, sizeof c );
	
	return bytes ? EXIT_SUCCESS : EXIT_FAILURE;
}

