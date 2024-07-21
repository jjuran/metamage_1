/*
	window.cc
	---------
*/

#include "window.hh"

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

// Chess
#include "Rez/app_name.h"


#pragma exceptions off


using mac::qd::main_display_bounds;


WindowRef main_window;

static const Byte window_title[] = "\p" APP_NAME;

static inline
short min( short a, short b )
{
	return b < a ? b : a;
}

short make_main_window()
{
	const short alignment = 16;
	
	Rect bounds = main_display_bounds();
	
	bounds.top    += 38;  // approximate menu bar height + title bar height
	bounds.bottom -=  2;  // approximate window frame thickness
	
	const short ideal_length = 37 * 8;
	
	const short usable_height = bounds.bottom - bounds.top;
	const short usable_width  = bounds.right - bounds.left;
	
	short usable_length = min( usable_height, usable_width );
	
	short length = usable_length < ideal_length ? usable_length : ideal_length;
	
	length -= length % 8u;  // make it an exact multiple
	
	const short v_margin = usable_height - length;
	const short h_margin = usable_width  - length;
	
	bounds.top   += v_margin / 2u;
	bounds.bottom = bounds.top + length;
	
	bounds.left += h_margin / 2u;
	bounds.right = bounds.left + length;
	
#if TARGET_API_MAC_CARBON
	
	const WindowAttributes attrs = kWindowCloseBoxAttribute
	                             | kWindowCollapseBoxAttribute
	                           #ifdef MAC_OS_X_VERSION_10_3
	                             | kWindowAsyncDragAttribute
	                           #endif
	                             ;
	
	OSStatus err;
	err = CreateNewWindow( kDocumentWindowClass, attrs, &bounds, &main_window );
	
	if ( err )
	{
		ExitToShell();
	}
	
	SetWTitle( main_window, window_title );
	
	ShowWindow( main_window );
	
#else
	
	main_window = NewWindow( NULL,
	                         &bounds,
	                         window_title,
	                         true,
	                         noGrowDocProc,
	                         (WindowRef) -1,
	                         true,
	                         0 );
	
#endif
	
	SetPortWindowPort( main_window );
	
	return length;
}
