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
	
	NN::Owned< N::ICInstance > ic = N::ICStart( signature );
	
#if !TARGET_API_MAC_CARBON
	
	N::ThrowOSStatus( ::ICFindConfigFile( ic, 0, NULL ) );
	
#endif
	
	N::ThrowOSStatus( ::ICBegin( ic, icReadWritePerm ) );
	
	NN::Owned< N::Handle > mappings = N::ICFindPrefHandle( ic, kICMapping );
	
	std::printf( "Mapping handle size: %d\n", N::GetHandleSize( mappings ) );
	
	::ICEnd( ic );
	
	return 0;
}

