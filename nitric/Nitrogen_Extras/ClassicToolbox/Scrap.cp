/*	========
 *	Scrap.cp
 *	========
 */

// *** A-line Processing Instructions ***
// API: ClassicMacToolbox (not Carbon)

#ifndef CLASSICTOOLBOX_SCRAP_H
#include "ClassicToolbox/Scrap.h"
#endif

// Nitrogen
#ifndef NITROGEN_ONLYONCE_H
#include "Nitrogen/OnlyOnce.h"
#endif
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif


namespace Nitrogen
{
	
#if CALL_NOT_IN_CARBON
	
	void ZeroScrap()
	{
		OnlyOnce< RegisterScrapManagerErrors >();
		
		ThrowOSStatus( ::ZeroScrap() );
	}
	
#endif
	
}

