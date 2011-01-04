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
#ifndef MAC_TOOLBOX_UTILITIES_THROWOSSTATUS_HH
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"
#endif


namespace Mac
{
	
	inline void ResError()
	{
		ThrowOSStatus( ::ResError() );
	}
	
}

#endif

