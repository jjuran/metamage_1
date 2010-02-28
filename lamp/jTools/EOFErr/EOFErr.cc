/*	=========
 *	EOFErr.cc
 *	=========
 */

// Nucleus
#include "Nucleus/Exception.h"
#include "Nucleus/TheExceptionBeingHandled.h"

// Nitrogen
#include "Nitrogen/MacErrors.hh"

// Orion
#include "Orion/Main.hh"


namespace N = Nitrogen;
namespace NN = Nucleus;
namespace O = Orion;


namespace Orion
{
	
	int Main( int argc, iota::argv_t argv )
	{
		NN::RegisterExceptionConversion< NN::Exception, N::OSStatus >();
		
		throw Nitrogen::EOFErr();
	}
	
}

