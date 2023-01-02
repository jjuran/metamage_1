/*
	get_portRect.hh
	---------------
*/

#ifndef MACQD_GETPORTRECT_HH
#define MACQD_GETPORTRECT_HH

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

#if OPAQUE_TOOLBOX_STRUCTS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif
#endif

// missing-macos
#if ! __LP64__
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#endif
#endif


namespace mac {
namespace qd  {
	
#if ! OPAQUE_TOOLBOX_STRUCTS
	
	inline
	const Rect& get_portRect( GrafPtr port )
	{
		return port->portRect;
	}
	
	inline
	const Rect& get_portRect( CGrafPtr port )
	{
		return port->portRect;
	}
	
#else
	
#if ! __LP64__
	
	inline
	Rect get_portRect( GrafPtr port )
	{
		Rect bounds;
		GetPortBounds( port, &bounds );
		
		return bounds;
	}
	
	inline
	Rect get_portRect( WindowRef window )
	{
		return get_portRect( GetWindowPort( window ) );
	}
	
#endif
#endif  // #if ! OPAQUE_TOOLBOX_STRUCTS
	
}
}

#endif
