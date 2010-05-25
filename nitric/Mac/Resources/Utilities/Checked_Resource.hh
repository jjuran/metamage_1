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
#include "Mac/Resources/Functions/ResError.hh"

#ifndef NITROGEN_OSSTATUS_HH
#include "Nitrogen/OSStatus.hh"
#endif


namespace Mac
{
	
	inline ::Handle Checked_Resource( ::Handle r )
	{
		if ( r == NULL )
		{
			ResError();
			
			Nitrogen::ThrowOSStatus( resNotFound );
		}
		
		return r;
	}
	
}

#endif

