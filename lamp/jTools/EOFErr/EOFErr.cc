/*	=========
 *	EOFErr.cc
 *	=========
 */

// Nucleus
#include "Nucleus/Exception.h"
#include "Nucleus/TheExceptionBeingHandled.h"

// Nitrogen
#include "Nitrogen/MacErrors.h"

// Orion
#include "Orion/Main.hh"


namespace N = Nitrogen;
namespace NN = Nucleus;
namespace O = Orion;


int O::Main( int argc, const char *const argv[] )
{
	NN::RegisterExceptionConversion< NN::Exception, N::OSStatus >();
	
	throw Nitrogen::EOFErr();
}

