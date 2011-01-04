/*	========
 *	Scrap.cc
 *	========
 */

// *** A-line Processing Instructions ***
// API: ClassicMacToolbox (not Carbon)

#include "ClassicToolbox/Scrap.hh"

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"


namespace Nitrogen
{
	
	void ZeroScrap()
	{
		Mac::ThrowOSStatus( ::ZeroScrap() );
	}
	
}

