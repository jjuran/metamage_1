/*	========
 *	Scrap.cp
 *	========
 */

// *** A-line Processing Instructions ***
// API: ClassicMacToolbox (not Carbon)

#include "ClassicToolbox/Scrap.h"

// Nitrogen
#include "Nitrogen/OSStatus.h"


namespace Nitrogen
{
	
	void ZeroScrap()
	{
		ThrowOSStatus( ::ZeroScrap() );
	}
	
}

