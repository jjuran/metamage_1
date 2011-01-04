/*
	Mac/Toolbox/Utilities/ThrowOSStatus.hh
	--------------------------------------
*/

#ifndef MAC_TOOLBOX_UTILITIES_THROWOSSTATUS_HH
#define MAC_TOOLBOX_UTILITIES_THROWOSSTATUS_HH

// Mac OS
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif


namespace Mac
{
	
	void ThrowOSStatus_Internal( ::OSStatus );
   
	inline void ThrowOSStatus( ::OSStatus err )
	{
		if ( err != noErr )
		{
			ThrowOSStatus_Internal( err );
		}
	}
	
}

#endif

