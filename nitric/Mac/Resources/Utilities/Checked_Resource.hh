/*
	Mac/Resources/Utilities/Checked_Resource.hh
	-------------------------------------------
*/

#ifndef MAC_RESOURCES_UTILITIES_CHECKEDRESOURCE_HH
#define MAC_RESOURCES_UTILITIES_CHECKEDRESOURCE_HH

// Mac OS
#ifndef __MACERRORS__
#include <MacErrors.h>
#endif
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

// Nitrogen
#ifndef MAC_RESOURCES_FUNCTIONS_RESERROR_HH
#include "Mac/Resources/Functions/ResError.hh"
#endif
#ifndef MAC_TOOLBOX_UTILITIES_THROWOSSTATUS_HH
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"
#endif


namespace Mac
{
	
	inline ::Handle Checked_Resource( ::Handle r )
	{
		if ( r == NULL )
		{
			ResError();
			
			ThrowOSStatus( resNotFound );
		}
		
		return r;
	}
	
}

#endif

