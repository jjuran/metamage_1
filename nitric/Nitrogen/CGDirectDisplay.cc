// Nitrogen/CGDirectDisplay.cc
// ---------------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include <TargetConditionals.h>

#if TARGET_RT_MAC_MACHO || TARGET_API_MAC_OSX

#include "Nitrogen/CGDirectDisplay.hh"

// Nitrogen
#include "Nitrogen/OSStatus.hh"


namespace Nitrogen
{
	
	Nucleus::Owned< CGDirectDisplayID > CGDisplayCapture( CGDirectDisplayID display )
	{
		if ( display == 0 )
		{
			display = ::CGMainDisplayID();
		}
		
		ThrowOSStatus( ::CGDisplayCapture( display ) );
		
		return Nucleus::Owned< CGDirectDisplayID >::Seize( display );
	}
	
}

#endif  // #if TARGET_RT_MAC_MACHO || TARGET_API_MAC_OSX

