/*	======
 *	tty.cc
 *	======
 */

// POSIX
#include "unistd.h"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"


namespace O = Orion;


int O::Main( int argc, char const *const argv[] )
{
	const char* name = ttyname( 0 );
	
	Io::Out << ( name ? name : "not a tty" ) << "\n";
	
	return 0;
}

