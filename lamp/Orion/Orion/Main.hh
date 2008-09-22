/*	=======
 *	Main.hh
 *	=======
 */

#ifndef ORION_MAIN_HH
#define ORION_MAIN_HH

// Iota
#include "iota/argv.hh"

// POSeven
#include "POSeven/types/exit_t.hh"


namespace Orion
{
	
	inline void ThrowExitStatus( int status )  { throw poseven::exit_t( status ); }
	
}

namespace tool
{
	
	int Main( int argc, iota::argv_t argv );
	
}

#endif

