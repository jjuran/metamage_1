/*
	create_IconRef_from_icns.cc
	---------------------------
*/

#include "mac_icon/create_IconRef_from_icns.hh"

// Mac OS
#ifndef __ICONS__
#include <Icons.h>
#endif


namespace mac  {
namespace icon {

IconRef create_IconRef_from_icns( const FSRef& file )
{
	const OSType creator = 0;
	const OSType type    = 0;
	
	OSStatus err;
	IconRef icon;
	
	err = RegisterIconRefFromFSRef( creator, type, &file, &icon );
	
	if ( err != noErr )
	{
		return NULL;
	}
	
	// This shouldn't fail.  If it does, something is seriously wrong.
	err = AcquireIconRef( icon );
	
	err = UnregisterIconRef( creator, type );
	
	return icon;
}

}
}
