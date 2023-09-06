/*
	Window.hh
	---------
*/

#ifndef WINDOW_HH
#define WINDOW_HH

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif

// mac-qd-utils
#include "mac_qd/main_display_bounds.hh"

// NyanochromeCat
#include "Geometry.hh"
#include "Offscreen.hh"


#pragma exceptions off


extern WindowRef main_window;

static inline
short aligned( short x, short alignment )
{
	const short antimask = alignment - 1;
	const short rounding = antimask / 2;
	
	return x + rounding & ~antimask;
}

inline
void make_main_window()
{
	const short alignment = 16;
	
	Rect bounds = mac::qd::main_display_bounds();
	
	/*
		Using unsigned division here assumes that the main display's bounds
		has non-negative coordinates (i.e. it hangs down and to the right of
		the origin), and that it's big enough to enclose the window bounds.
		
		(We expect that the screen will be no smaller than 512x342,
		and our window is sized to fit within that area.)
	*/
	
	const short exact_left = (bounds.left + bounds.right - nyan_width ) / 2u;
	
	const short aligned_left = aligned( exact_left, alignment );
	
	bounds.left  = aligned_left;
	bounds.right = bounds.left + nyan_width;
	
	bounds.top    = (bounds.top + bounds.bottom - nyan_height - 41) / 2u + 39;
	bounds.bottom = bounds.top + nyan_height;
	
	ConstStr255Param title = "\p" "Nyanochrome Cat";
	
	if ( TARGET_API_MAC_CARBON )
	{
		const WindowAttributes attrs = kWindowCloseBoxAttribute
		                             | kWindowCollapseBoxAttribute
		                             | kWindowStandardHandlerAttribute
		                           #ifdef MAC_OS_X_VERSION_10_3
		                             | kWindowAsyncDragAttribute
		                           #endif
		                             ;
		
		OSStatus err;
		WindowRef window;
		err = CreateNewWindow( kDocumentWindowClass, attrs, &bounds, &window );
		
		if ( err != noErr )
		{
			ExitToShell();
		}
		
		SetWTitle( window, title );
		
		ShowWindow( window );
		
		main_window = window;
	}
	else
	{
		main_window = NewWindow( NULL,
		                         &bounds,
		                         title,
		                         true,
		                         noGrowDocProc,
		                         (WindowRef) -1,
		                         true,
		                         0 );
	}
	
	SetPortWindowPort( main_window );
}

inline
void draw_window( WindowRef window )
{
	blit( GetWindowPort( window ) );
}

#endif
