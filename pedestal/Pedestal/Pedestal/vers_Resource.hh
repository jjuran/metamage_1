/*
	vers_Resource.hh
	----------------
*/

#ifndef PEDESTAL_VERSRESOURCE_HH
#define PEDESTAL_VERSRESOURCE_HH

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
	
	bool Get_vers_ShortVersionString( short id, Str255 data );
	
}

#endif
