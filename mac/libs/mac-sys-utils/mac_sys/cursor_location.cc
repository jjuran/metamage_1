/*
	cursor_location.cc
	------------------
*/

#include "mac_sys/cursor_location.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

#if ! TARGET_API_MAC_CARBON

// Mac OS
#ifndef __CURSORDEVICES__
#include <CursorDevices.h>
#endif

// mac-sys-utils
#include "mac_sys/has/CursorDevices.hh"


Point MouseTemp : 0x0828;
Point RawMouse  : 0x082C;

short CrsrNew_CrsrCouple : 0x8ce;


namespace mac {
namespace sys {

Point get_cursor_location()
{
	if ( ! TARGET_CPU_68K  ||  has_CursorDevices() )
	{
		CursorDevicePtr device = NULL;
		
		OSErr err = CursorDeviceNextDevice( &device );
		
		if ( err == noErr  &&  device->whichCursor != NULL )
		{
			return device->whichCursor->where;
		}
	}
	
	return RawMouse;
}

void set_cursor_location( Point location )
{
	if ( ! TARGET_CPU_68K  ||  has_CursorDevices() )
	{
		CursorDevicePtr device = NULL;
		
		OSErr err = CursorDeviceNextDevice( &device );
		
		if ( err == noErr )
		{
			CursorDeviceMoveTo( device, location.h, location.v );
		}
	}
	else
	{
		RawMouse  = location;
		MouseTemp = location;
		
		CrsrNew_CrsrCouple = -1;
	}
}

}
}

#elif defined( __APPLE__ )

int dummy;

#endif  // #if ! TARGET_API_MAC_CARBON
