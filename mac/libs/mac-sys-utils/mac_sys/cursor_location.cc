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
#ifndef __LOWMEM__
#include <LowMem.h>
#endif

// mac-sys-utils
#include "mac_sys/has/CursorDevices.hh"


#if TARGET_CPU_68K

short CrsrNew_CrsrCouple : 0x8ce;

#else

static short CrsrNew_CrsrCouple;

#endif


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
	
	return LMGetRawMouseLocation();
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
		LMSetRawMouseLocation( location );
		LMSetMouseTemp       ( location );
		
		CrsrNew_CrsrCouple = -1;
	}
}

}
}

#elif defined( __APPLE__ )

int dummy;

#endif  // #if ! TARGET_API_MAC_CARBON
