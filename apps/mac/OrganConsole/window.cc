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
#include "mac_qd/globals/patterns.hh"
#include "mac_qd/main_display_bounds.hh"


#pragma exceptions off


const short n_keys_wide = 13;  // A3 through F5 in upper manual

static short key_width;

static
Rect get_window_bounds()
{
	Rect bounds = mac::qd::main_display_bounds();
	
	bounds.top    += 40;  // approximate menu bar height + title bar height
	bounds.bottom -=  2;  // approximate window frame thickness
	
	short desk_height = bounds.bottom - bounds.top;
	short desk_width  = bounds.right - bounds.left;
	
	key_width = desk_width < n_keys_wide * 32 ? 20  // fits 320x240
	          : desk_width < n_keys_wide * 64 ? 32  // fits 512x342
	          :                                 48;
	
	short window_width = key_width * n_keys_wide;
	
	short window_height = window_width * 11 / 16u;
	
	short v_margin = (desk_height - window_height) / 2u;
	short h_margin = (desk_width  - window_width)  / 2u;
	
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
	const short padded_width = manual_width * 16 / n_keys_wide;
	
	short bottom = bounds.bottom;
	short midway = bounds.bottom / 2u;
	
	Rect key_rect = bounds;
	
	key_rect.right = padded_width / 16u;
	
	for ( int i = 0;  i < n_keys_wide;  ++i )
	{
		FrameRect( &key_rect );
		
		key_rect.left   = key_rect.right;
		key_rect.right += key_width;
	}
	
	key_rect.top   = midway;
	key_rect.left  = 0;
	key_rect.right = padded_width / 16u;
	
	FillRect( &key_rect, &mac::qd::ltGray() );
	FrameRect( &key_rect );
	
	key_rect.left  = padded_width * 11 / 16u;
	key_rect.right = bounds.right;
	
	FillRect( &key_rect, &mac::qd::ltGray() );
	FrameRect( &key_rect );
	
	MoveTo( 0, key_rect.top );
	Line  ( manual_width, 0 );
}

static inline
void draw_black_keys( const Rect& bounds )
{
	const short manual_width = bounds.right - bounds.left;
	const short padded_width = manual_width * 16 / n_keys_wide;
	
	Rect key_rect = bounds;
	
	key_rect.bottom = bounds.bottom * 5 / 8u / 2;
	key_rect.left   = padded_width / 32u;
	
	for ( int i = 1;  i <= 11;  ++i )
	{
		if ( i == 2  ||  i == 5  ||  i == 9 )
		{
			continue;
		}
		
		const short edge = padded_width * i / 16u;
		
		key_rect.left  = edge - padded_width / 64u;
		key_rect.right = edge + padded_width / 64u;
		
		PaintRect( &key_rect );
	}
	
	OffsetRect( &key_rect, 0, bounds.bottom / 2u );
	
	key_rect.left = padded_width / 32u;
	
	for ( int i = 3;  i <= 10;  ++i )
	{
		if ( i == 5  ||  i == 9 )
		{
			continue;
		}
		
		const short edge = padded_width * i / 16u;
		
		key_rect.left  = edge - padded_width / 64u;
		key_rect.right = edge + padded_width / 64u;
		
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
