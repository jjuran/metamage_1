/*
	Mac/Resources/Functions/ResError.hh
	-----------------------------------
*/

#ifndef MAC_RESOURCES_FUNCTIONS_RESERROR_HH
#define MAC_RESOURCES_FUNCTIONS_RESERROR_HH

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// Nitrogen
#ifndef NITROGEN_OSSTATUS_HH
#include "Nitrogen/OSStatus.hh"
#endif


namespace Mac
{
	
	inline void ResError()
	{
		Nitrogen::ThrowOSStatus( ::ResError() );
	}
	
}

#endif

