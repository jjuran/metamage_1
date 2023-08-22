/*
	welcome.cc
	----------
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
#include "missing/Quickdraw.h"
#endif
#ifndef MISSING_QUICKDRAWTEXT_H
#include "missing/QuickdrawText.h"
#endif
#endif

// mac-sys-utils
#include "mac_sys/gestalt.hh"
#include "mac_sys/trap_available.hh"

// mac-qd-utils
#include "mac_qd/get_portRect.hh"
#include "mac_qd/main_display_bounds.hh"
#include "mac_qd/plot_icon_id.hh"

// mac-app-utils
#include "mac_app/new_window.hh"


#if TARGET_API_MAC_CARBON
#define SystemTask()  /**/
#endif

#define MBarHeight  *(short*) 0x0BAA  // only used in v68k for AMS


using mac::qd::get_portRect;
using mac::qd::main_display_bounds;


static inline
bool in_v68k()
{
	return mac::sys::gestalt_defined( 'v68k' );
}

static
WindowRef create_window()
{
	using mac::app::new_window;
	
	Rect bounds = main_display_bounds();
	
	const short width  = 428;
	const short height = 112;
	
	bounds.left  = (bounds.right - width) / 2;
	bounds.right = bounds.left + width;
	
	bounds.top    = (bounds.bottom - height) / 3 - 4;
	bounds.bottom = bounds.top + height;
	
	return new_window( bounds, "\p", true, dBoxProc, (WindowRef) -1, false );
}

#define WELCOME( name ) "\p" "Welcome to " name "."

static
void draw_window( WindowRef window )
{
	const Rect& portRect = get_portRect( window );
	
	SetPortWindowPort( window );
	
	EraseRect( &portRect );
	
	SInt32 micn = mac::sys::gestalt( 'micn' );
	
	const short icon_h = 16;
	const short icon_v = 16;
	
	const Rect icon_rect = { icon_v, icon_h, icon_v + 32, icon_h + 32 };
	
	mac::qd::plot_icon_id( icon_rect, micn );
	
	const short text_h = 100;
	const short text_v = 36;
	
	MoveTo( text_h, text_v );
	
	DrawString( "\p" "Welcome to Advanced Mac Substitute." );
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
	
	if ( TARGET_CPU_68K  &&  in_v68k() )
	{
		MBarHeight = 0;
	}
	
	InitGraf( &qd.thePort );
	
	InitFonts();
	InitWindows();
	InitMenus();
	
	InitCursor();
	
	if ( TARGET_CPU_68K  &&  in_v68k() )
	{
		HideCursor();
	}
	
#endif
	
	WindowRef main_window = create_window();
	
#ifndef __APPLE__
	
	TextFont( systemFont );
	
#endif
	
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
				case keyDown:
					quitting = true;
					break;
				
				case updateEvt:
					window = (WindowRef) event.message;
					BeginUpdate( window );
					
					draw_window( window );
					
					EndUpdate  ( window );
					break;
				
				default:
					break;
			}
		}
	}
	
	return 0;
}
