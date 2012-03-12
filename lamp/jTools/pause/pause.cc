/*	========
 *	pause.cc
 *	========
 */

// POSIX
#include <unistd.h>

// Iota
#include "iota/strings.hh"


int main( int argc, char** argv )
{
	ssize_t bytes = write( STDOUT_FILENO, STR_LEN( "Hit return to continue\n" ) );
	
	char c;
	
	bytes = read( STDIN_FILENO, &c, sizeof c );
	
	return bytes ? 0 : 1;
}

