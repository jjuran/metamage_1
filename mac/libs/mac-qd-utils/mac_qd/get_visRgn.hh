/*
	get_visRgn.hh
	-------------
*/

#ifndef MACQD_GETVISRGN_HH
#define MACQD_GETVISRGN_HH

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


namespace mac {
namespace qd  {
	
#if ! OPAQUE_TOOLBOX_STRUCTS
	
	inline
	::RgnHandle get_visRgn( GrafPtr port )
	{
		return port->visRgn;
	}
	
	inline
	::RgnHandle get_visRgn( CGrafPtr port )
	{
		return port->visRgn;
	}
	
#else
	
#if ! __LP64__
	
	::RgnHandle get_visRgn( GrafPtr port );
	
	inline
	::RgnHandle get_visRgn( WindowRef window )
	{
		return get_visRgn( GetWindowPort( window ) );
	}
	
#endif
#endif  // #if ! OPAQUE_TOOLBOX_STRUCTS
	
}
}

#endif
