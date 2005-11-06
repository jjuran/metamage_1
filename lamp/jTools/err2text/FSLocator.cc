/*	============
 *	FSLocator.cc
 *	============
 */

#include "FSLocator.hh"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"


namespace FSLocator
{
	
	namespace NX = NitrogenExtras;
	
	FSSpec FSLocatorAppBySignature::Locate() const
	{
		return NX::DTGetAPPL( mySig );
	}
	
}

