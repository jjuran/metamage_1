/*	=============
 *	MemFullErr.cc
 *	=============
 */

// Nitrogen
#include "Nitrogen/MacErrors.h"

// Orion
#include "Orion/Main.hh"


namespace O = Orion;


int O::Main( int argc, argv_t argv )
{
	throw Nitrogen::MemFullErr();
}

