/*
	Calculator.cc
	-------------
*/

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
#include "missing/Fonts.h"
#include "missing/Quickdraw.h"
#include "missing/QuickdrawText.h"
#endif
#endif

// mac-sys-utils
#include "mac_sys/trap_available.hh"

// mac-qd-utils
#include "mac_qd/get_portRect.hh"
#include "mac_qd/globals/patterns.hh"
#include "mac_qd/main_display_bounds.hh"
#include "mac_qd/wide_drag_area.hh"


#pragma exceptions off


#if TARGET_API_MAC_CARBON
#define SystemTask()  /**/
#define rDocProc noGrowDocProc
#endif

using mac::qd::get_portRect;
using mac::qd::main_display_bounds;
using mac::qd::wide_drag_area;


static
WindowRef make_window()
{
	const short width  = 102;
	const short height = 146;
	
	Rect bounds = main_display_bounds();
	
	bounds.left  = (bounds.right - width) / 2u;
	bounds.right = bounds.left + width;
	
	bounds.top    = (bounds.bottom - height) * 3 / 8u;
	bounds.bottom = bounds.top + height;
	
	WindowRef window = NewWindow( NULL,
	                              &bounds,
	                              "\p" "Calculator",
	                              true,
	                              rDocProc,
	                              (WindowRef) -1,
	                              true,
	                              0 );
	
	SetPortWindowPort( window );
	
	BackPat( &mac::qd::ltGray() );
	
	TextFont( kFontIDGeneva );
	TextSize( 9 );
	
	return window;
}

struct button_desc
{
	char name;
	char x, y;
	char code;
	char extra_width, extra_height;
};

static button_desc buttons[] =
{
	{ 'C', 0, 0 },
	{ '=', 1, 0 },
	{ '/', 2, 0 },
	{ '*', 3, 0 },
	{ '7', 0, 1 },
	{ '8', 1, 1 },
	{ '9', 2, 1 },
	{ '-', 3, 1 },
	{ '4', 0, 2 },
	{ '5', 1, 2 },
	{ '6', 2, 2 },
	{ '+', 3, 2 },
	{ '1', 0, 3 },
	{ '2', 1, 3 },
	{ '3', 2, 3 },
	{ '=', 3, 3, '\3', 0, 1 },
	{ '0', 0, 4, '\0', 1, 0 },
	{ '.', 2, 4 },
};

static inline
short button_top( short y )
{
	return 26 + 6 + y * (16 + 6);
}

static inline
short button_left( short x )
{
	return 8 + x * (18 + 4);
}

static inline
short button_bottom( short y )
{
	return button_top( y ) + 16;
}

static inline
short button_right( short x )
{
	return button_left( x ) + 18;
}

static
void draw_window( const Rect& rect )
{
	EraseRect( &rect );
	
	Rect console = { 8, 8, 26, 92 };
	
	FillRect( &console, &mac::qd::white() );
	
	InsetRect( &console, -1, -1 );
	FrameRect( &console );
	
	const button_desc* end = buttons + sizeof buttons / sizeof buttons[ 0 ];
	
	for ( const button_desc* it = buttons;  it < end;  ++it )
	{
		short x = it->x;
		short y = it->y;
		
		const short top  = button_top ( y );
		const short left = button_left( x );
		
		x += it->extra_width;
		y += it->extra_height;
		
		const short bottom = button_bottom( y );
		const short right  = button_right ( x );
		
		Rect r = { top, left, bottom, right };
		
		FillRect( &r, &mac::qd::white() );
		FrameRect( &r );
		
		r.top   += 2;
		r.left   = right;
		r.right += 2;
		
		PaintRect( &r );
		
		r.top     = bottom;
		r.left    = left + 2;
		r.bottom += 2;
		
		PaintRect( &r );
		
		MoveTo( left + 6, bottom - 4 );
		
		DrawChar( it->name );
	}
}

static inline
bool has_WaitNextEvent()
{
	enum { _WaitNextEvent = 0xA860 };
	
	return ! TARGET_CPU_68K  ||  mac::sys::trap_available( _WaitNextEvent );
}

static inline
Boolean WaitNextEvent( EventRecord& event )
{
	return WaitNextEvent( everyEvent, &event, 0x7FFFFFFF, NULL );
}

static inline
Boolean GetNextEvent( EventRecord& event )
{
	SystemTask();
	
	return GetNextEvent( everyEvent, &event );
}

int main()
{
	Boolean quitting = false;
	
#if ! TARGET_API_MAC_CARBON
	
	InitGraf( &qd.thePort );
	
	InitFonts();
	InitWindows();
	InitMenus();
	
#endif
	
#ifndef __APPLE__
	
	InitCursor();
	
#endif
	
	WindowRef main_window = make_window();
	
	const bool has_WNE = has_WaitNextEvent();
	
	while ( ! quitting )
	{
		EventRecord event;
		
		if ( has_WNE ? WaitNextEvent( event ) : GetNextEvent( event ) )
		{
			WindowRef window;
			
			switch ( event.what )
			{
				case mouseDown:
					switch ( FindWindow( event.where, &window ) )
					{
						case inDrag:
							DragWindow( window, event.where, wide_drag_area() );
							break;
						
						case inGoAway:
							if ( TrackGoAway( window, event.where ) )
							{
								DisposeWindow( main_window );
								
								main_window = NULL;
								quitting = true;
							}
						
						default:
							break;
					}
					break;
					
				case updateEvt:
					window = (WindowRef) event.message;
					BeginUpdate( window );
					
					SetPortWindowPort( window );
					
					draw_window( get_portRect( window ) );
					
					EndUpdate  ( window );
					break;
				
				default:
					break;
			}
		}
	}
	
	ExitToShell();
	
	return 0;
}
