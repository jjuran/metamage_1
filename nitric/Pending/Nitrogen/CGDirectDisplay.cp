/*	==================
 *	CGDirectDisplay.cp
 *	==================
 */

#include <TargetConditionals.h>

#if TARGET_RT_MAC_MACHO || TARGET_API_MAC_OSX

#ifndef NITROGEN_CGDIRECTDISPLAY_H
#include "Nitrogen/CGDirectDisplay.h"
#endif

#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif

namespace Nitrogen
{
	
	Owned< CGDirectDisplayID > CGDisplayCapture( CGDirectDisplayID display )
	{
		if ( display == 0 )
		{
			display = ::CGMainDisplayID();
		}
		
		ThrowOSStatus( ::CGDisplayCapture( display ) );
		
		return Owned< CGDirectDisplayID >::Seize( display );
	}
	
}

#endif  // #if TARGET_RT_MAC_MACHO || TARGET_API_MAC_OSX

