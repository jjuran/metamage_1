/*	======
 *	env.cc
 *	======
 */

// POSIX
#include <unistd.h>

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"


namespace O = Orion;


static void DumpEnvironment()
{
	for ( const char* const* envp = environ;  *envp != NULL;  ++envp )
	{
		Io::Out << *envp << "\n";
	}
}

int O::Main( int argc, char const *const /*argv*/[] )
{
	if ( argc == 1  &&  environ != NULL )
	{
		DumpEnvironment();
	}
	
	return 0;
}

