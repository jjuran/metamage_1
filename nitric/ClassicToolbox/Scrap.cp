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
	
#if CALL_NOT_IN_CARBON
	
	void ZeroScrap()
	{
		(void) ScrapManagerErrorsRegistrationDependency();
		
		ThrowOSStatus( ::ZeroScrap() );
	}
	
#endif
	
}

