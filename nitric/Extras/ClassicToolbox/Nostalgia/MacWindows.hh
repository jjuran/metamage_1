// Nostalgia/MacWindows.hh

#ifndef NOSTALGIA_MACWINDOWS_HH
#define NOSTALGIA_MACWINDOWS_HH

#ifndef __MACWINDOWS__
#include <MacWindows.h>
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

