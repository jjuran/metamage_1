/*
	checked_resource.cc
	-------------------
*/

#include "Genie/Utilities/checked_resource.hh"

// mac-sys-utils
#include "mac_sys/res_error.hh"

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"


namespace Genie
{

enum
{
	resNotFound = -192,
};

Handle checked_resource( Handle h )
{
	using mac::sys::res_error;
	
	if ( ! h )
	{
		OSErr err = res_error();
		
		if ( err == noErr )
		{
			err = resNotFound;
		}
		
		Mac::ThrowOSStatus( err );
	}
	
	return h;
}

}
