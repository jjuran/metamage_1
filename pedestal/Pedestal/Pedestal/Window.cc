/*
	Window.cc
	---------
*/

#include "Pedestal/Window.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif

// mac-qd-utils
#include "mac_qd/get_portRect.hh"

// Pedestal
#include "Pedestal/View.hh"
#include "Pedestal/WindowStorage.hh"


#if TARGET_API_MAC_CARBON
#define NONCARBON_INLINE  /**/
#else
#define NONCARBON_INLINE  inline
#endif


namespace Pedestal
{
	
	static NONCARBON_INLINE
	void ResizedWindow( WindowRef window )
	{
		using mac::qd::get_portRect;
		
		// Don't rely on the requested size because it might have been tweaked
		
		// Shotgun approach -- invalidate the whole window.
		// This conveniently includes both old and new grow box locations.
		// Clients can validate regions if they want.
		
		invalidate_window( window );
		
		if ( View* view = get_window_view( window ) )
		{
			view->SetBounds( get_portRect( window ) );
		}
		
		if ( WindowResized_proc proc = get_window_resized_proc( window ) )
		{
			proc( window );
		}
	}
	
	void ResizeWindow( WindowRef window, Point newSize )
	{
		SizeWindow( window, newSize.h, newSize.v, true );
		
		ResizedWindow( window );
	}
	
	void ResizingWindow( WindowRef window, Point start )
	{
		if ( TARGET_API_MAC_CARBON )
		{
			if ( bool changed = ::ResizeWindow( window, start, NULL, NULL ) )
			{
				if ( GetWindowKind( window ) == kApplicationWindowKind )
				{
					ResizedWindow( window );
				}
			}
			
			return;
		}
		
		Rect sizeRect = { 30, 50, 10000, 10000 };
		
		if ( long grown = GrowWindow( window, start, &sizeRect ) )
		{
			if ( GetWindowKind( window ) == kApplicationWindowKind )
			{
				ResizeWindow( window, (Point&) grown );
			}
		}
	}
	
}
