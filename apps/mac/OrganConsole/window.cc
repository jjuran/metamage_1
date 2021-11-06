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

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#endif

// mac-qd-utils
#include "mac_qd/get_portRect.hh"
#include "mac_qd/main_display_bounds.hh"


#pragma exceptions off


static
Rect get_window_bounds()
{
	Rect bounds = mac::qd::main_display_bounds();
	
	bounds.top    += 40;  // approximate menu bar height + title bar height
	bounds.bottom -=  2;  // approximate window frame thickness
	
	short desk_height = bounds.bottom - bounds.top;
	short desk_width  = bounds.right - bounds.left;
	
	short v_margin = desk_height / 4u;
	short h_margin = desk_width  / 4u;
	
	bounds.top += v_margin;
	bounds.left += h_margin;
	bounds.right -= h_margin;
	bounds.bottom -= v_margin;
	
	return bounds;
}

static inline
WindowRef new_window( ConstStr255Param name )
{
	Rect bounds = get_window_bounds();
	
	if ( TARGET_API_MAC_CARBON )
	{
		const WindowAttributes attrs = kWindowCloseBoxAttribute
		                             | kWindowCollapseBoxAttribute
		                           #ifdef MAC_OS_X_VERSION_10_3
		                             | kWindowAsyncDragAttribute
		                           #endif
		                             ;
		
		OSStatus err;
		WindowRef window;
		
		err = CreateNewWindow( kDocumentWindowClass, attrs, &bounds, &window );
		
		if ( err != noErr )
		{
			return NULL;
		}
		
		SetWTitle( window, name );
		
		ShowWindow( window );
		
		return window;
	}
	
	const WindowRef behind = (WindowRef) -1;
	const short     procid = noGrowDocProc;
	
	return NewWindow( NULL, &bounds, name, true, procid, behind, true, 0 );
}

WindowRef create_window()
{
	return new_window( "\p" "Organ Console" );
}

static inline
void draw_white_keys( const Rect& bounds )
{
	const short manual_width = bounds.right - bounds.left;
	
	Rect key_rect = bounds;
	
	for ( int i = 1;  i <= 8;  ++i )
	{
		key_rect.right = manual_width * i / 8u;
		
		FrameRect( &key_rect );
		
		key_rect.left = key_rect.right;
	}
}

static inline
void draw_black_keys( const Rect& bounds )
{
	const short manual_width = bounds.right - bounds.left;
	
	Rect key_rect = bounds;
	
	key_rect.bottom  = bounds.bottom * 5 / 8u;
	key_rect.left   += manual_width / 16u;
	
	for ( int i = 1;  i < 7;  ++i )
	{
		if ( i == 3 )
		{
			continue;
		}
		
		const short edge = manual_width * i / 8u;
		
		key_rect.left  = edge - manual_width / 32u;
		key_rect.right = edge + manual_width / 32u;
		
		PaintRect( &key_rect );
	}
}

void draw_window( WindowRef window )
{
	const Rect& bounds = mac::qd::get_portRect( window );
	
	SetPortWindowPort( window );
	
	EraseRect( &bounds );
	
	draw_white_keys( bounds );
	draw_black_keys( bounds );
}
