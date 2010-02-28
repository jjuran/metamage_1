/*	========
 *	Scrap.cc
 *	========
 */

// *** A-line Processing Instructions ***
// API: ClassicMacToolbox (not Carbon)

#include "ClassicToolbox/Scrap.hh"

// Nitrogen
#include "Nitrogen/OSStatus.hh"


namespace Nitrogen
{
	
	void ZeroScrap()
	{
		ThrowOSStatus( ::ZeroScrap() );
	}
	
}

