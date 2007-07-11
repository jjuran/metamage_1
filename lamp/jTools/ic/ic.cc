/*	=====
 *	ic.cc
 *	=====
 */

// Nitrogen
#include "Nitrogen/InternetConfig.h"

// Orion
#include "Orion/Main.hh"


namespace N = Nitrogen;
namespace NN = Nucleus;
namespace O = Orion;


int O::Main( int argc, const char *const argv[] )
{
	NN::RegisterExceptionConversion< NN::Exception, N::OSStatus >();
	
	N::OSType signature = N::OSType( 'Poof' );
	
	N::ICInstance ic = N::ICStart( signature );
	
	N::ThrowOSStatus( ::ICBegin( ic, icReadWritePerm ) );
	
	//N::ICFindPrefHandle( ic, kICMapping );
	
	::ICEnd( ic );
	
	return 0;
}

