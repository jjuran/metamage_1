/*
	OwnerResource.hh
	----------------
*/

#ifndef PEDESTAL_OWNERRESOURCE_HH
#define PEDESTAL_OWNERRESOURCE_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif


namespace Pedestal
{
	
	OSType GetCreatorFromBNDL();
	
	bool GetOwnerResourceData( OSType creator, Str255 data );
	
}

#endif
