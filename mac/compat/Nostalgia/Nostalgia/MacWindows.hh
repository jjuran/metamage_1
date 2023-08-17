/*
	Nostalgia/MacWindows.hh
	-----------------------
*/

#ifndef NOSTALGIA_MACWINDOWS_HH
#define NOSTALGIA_MACWINDOWS_HH

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif

// missing-macos
#if ! __LP64__
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if TARGET_API_MAC_CARBON

inline pascal void InvalRect( const Rect* r )
{
	::InvalWindowRect( ::GetWindowFromPort( ::GetQDGlobalsThePort() ), r );
}

inline pascal void InvalRgn( RgnHandle rgn )
{
	::InvalWindowRgn( ::GetWindowFromPort( ::GetQDGlobalsThePort() ), rgn );
}

inline pascal void ValidRect( const Rect* r )
{
	::ValidWindowRect( ::GetWindowFromPort( ::GetQDGlobalsThePort() ), r );
}

inline pascal void ValidRgn( RgnHandle rgn )
{
	::ValidWindowRgn( ::GetWindowFromPort( ::GetQDGlobalsThePort() ), rgn );
}

#endif

#ifdef __cplusplus
}
#endif

#endif
