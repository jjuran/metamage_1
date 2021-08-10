/*
	Tic-tac-toe.cc
	--------------
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
#endif

// mac-config
#include "mac_config/apple-events.hh"
#include "mac_config/desk-accessories.hh"

// mac-sys-utils
#include "mac_sys/gestalt.hh"
#include "mac_sys/trap_available.hh"

// mac-qd-utils
#include "mac_qd/get_portRect.hh"
#include "mac_qd/main_display_bounds.hh"
#include "mac_qd/wide_drag_area.hh"

// mac-ui-utils
#include "mac_ui/menus.hh"

// mac-app-utils
#include "mac_app/DAs.hh"
#include "mac_app/event_handlers.hh"
#include "mac_app/init.hh"
#include "mac_app/menus.hh"
#include "mac_app/state.hh"

// Tic-tac-toe
#include "fullscreen.hh"
#include "menus.hh"
#include "state.hh"
#include "window.hh"


#define CONFIG_DAs CONFIG_DESK_ACCESSORIES

#if TARGET_API_MAC_CARBON
#define SystemTask()  /**/
#endif

using mac::qd::get_portRect;
using mac::qd::main_display_bounds;
using mac::qd::wide_drag_area;


// gestaltAppleEventsAttr = 'evnt'

const bool apple_events_present =
	CONFIG_APPLE_EVENTS  &&
		(CONFIG_APPLE_EVENTS_GRANTED  ||
			mac::sys::gestalt( 'evnt' ) != 0);

static bool sound_enabled;
static bool is_fullscreen;

static inline
void InvalRect( WindowRef window, const Rect& rect )
{
#if ! TARGET_API_MAC_CARBON
	
	InvalRect( &rect );
	return;
	
#endif
	
	InvalWindowRect( window, &rect );
}

static const Rect grow_size =
{
	50, 50,
	32767, 32767,
};

static RgnHandle gMouseRgn;

/*
	      *     *
	      *     *
	  * * * * * * * *
	      *     *
	      *     *
	  * * * * * * * *
	      *     *
	      *     *
	
	| |<-     3u
	 |<-->|   8u
	    -> <- 1u
	
	3 + 8 + 1 + 8 + 1 + 8 + 3 == 32
*/

enum
{
	kMargin = 3,
};

static WindowRef main_window;

static
void make_main_window()
{
	const short alignment = 16;
	
	Rect bounds = main_display_bounds();
	
	bounds.top    += 40;  // approximate menu bar height + title bar height
	bounds.bottom -=  8;  // approximate window frame thickness
	
	const short usableHeight = bounds.bottom - bounds.top;
	
	short height = usableHeight;
	
	height -= height /  4u;  // leave some space
	height -= height % 16u;  // make it an exact multiple
	
	const short margin = usableHeight - height;
	
	bounds.top   += margin / 2u;
	bounds.bottom = bounds.top + height;
	
	bounds.left  = (bounds.right - bounds.left - height) / 2u;
	bounds.right = bounds.left + height;
	
	main_window = NewWindow( NULL,
	                         &bounds,
	                         "\p" "Tic-tac-toe",
	                         true,
	                         documentProc,
	                         (WindowRef) -1,
	                         true,
	                         0 );
	
	SetPortWindowPort( main_window );
}

static
void draw_window( WindowRef window )
{
	draw_window( get_portRect( window ) );
}

static
void click( Point where )
{
	if ( RgnHandle rgn = click( where, sound_enabled ) )
	{
		gMouseRgn = rgn;
		
		propagate_to_dock_tile();
		
		mac::ui::enable_menu_item( Edit_menu, Undo );
	}
}

static
void invalidate_window_size( WindowRef window )
{
	const Rect& portRect = get_portRect( window );
	
	InvalRect( window, portRect );
	
	window_size_changed( portRect );
}

static
void enter_fullscreen()
{
	fullscreen::enter();
	
	ForeColor( whiteColor );
	BackColor( blackColor );
	
	const Rect& portRect = main_display_bounds();
	
	window_size_changed( portRect );
	
	draw_window( portRect );
}

static
void leave_fullscreen()
{
	fullscreen::leave();
	
	invalidate_window_size( main_window );
}

static
void cleanup_fullscreen()
{
	if ( TARGET_CPU_68K  &&  mac::sys::gestalt_defined( 'v68k' ) )
	{
		return;
	}
	
	if ( TARGET_API_MAC_CARBON  &&  mac::sys::gestalt( 'sysv' ) >= 0x1000 )
	{
		return;
	}
	
	fullscreen::leave();
}

static
void menu_item_chosen( long choice )
{
	short menu = choice >> 16;
	short item = choice;
	
	switch ( menu )
	{
		case Apple:
			if ( item == About )
			{
				// About...
			}
			else if ( ! TARGET_API_MAC_CARBON )
			{
				mac::app::open_DA_from_menu( item );
			}
			break;
		
		case File:
			switch ( item )
			{
				case NewGame:
					gMouseRgn = reset();
					
					propagate_to_dock_tile();
					break;
				
				case Open:
					break;
				
				case Close:
					if ( CONFIG_DAs  &&  mac::app::close_front_DA() )
					{
						break;
					}
					// fall through
				
				case Quit:
					mac::app::quitting = true;
					break;
				
				default:
					break;
			}
		
		case Edit:
			switch ( item )
			{
				case Undo:
					if ( RgnHandle rgn = undo() )
					{
						gMouseRgn = rgn;
					}
					
					propagate_to_dock_tile();
					
					if ( ! tictactoe::can_undo() )
					{
						mac::ui::disable_menu_item( Edit_menu, Undo );
					}
					
					break;
				
				default:
					break;
			}
			break;
		
		case Options:
			switch ( item )
			{
				case Sound:
					sound_enabled = ! sound_enabled;
					
					CheckMenuItem( Options_menu, item, sound_enabled );
					break;
				
				case Fullscreen:
					HiliteMenu( 0 );
					
					is_fullscreen = ! is_fullscreen;
					
					if ( is_fullscreen )
					{
						enter_fullscreen();
					}
					else
					{
						leave_fullscreen();
					}
					
					CheckMenuItem( Options_menu, item, is_fullscreen );
					break;
				
				default:
					break;
			}
			break;
		
		default:
			break;
	}
	
	HiliteMenu( 0 );
}

#ifdef __MC68K__

UInt8 SdVolume : 0x0260;
UInt8 SdEnable : 0x0261;

#else

const UInt8 SdVolume = 0;
const UInt8 SdEnable = 0;

#endif

static
void set_up_Options_menu()
{
#if ! TARGET_API_MAC_CARBON
	
	if ( SdVolume > 0 )
	{
		sound_enabled = true;
		
		CheckMenuItem( Options_menu, Sound, sound_enabled );
	}
	else if ( ! SdEnable )
	
#endif
	
	{
		mac::ui::disable_menu_item( Options_menu, Sound );
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
	return WaitNextEvent( everyEvent, &event, 0x7FFFFFFF, gMouseRgn );
}

static
Boolean wait_next_event( EventRecord& event )
{
	SystemTask();
	
	Boolean got = GetNextEvent( everyEvent, &event );
	
	if ( event.what == nullEvent  &&  ! PtInRgn( event.where, gMouseRgn ) )
	{
		event.what    = osEvt;
		event.message = mouseMovedMessage << 24;
		
		return true;
	}
	
	return got;
}

int main()
{
	using mac::app::quitting;
	
	mac::app::init_toolbox();
	mac::app::install_menus();
	
	set_up_menus();
	set_up_Options_menu();
	
	if ( apple_events_present )
	{
		mac::app::install_basic_event_handlers();
	}
	
	gMouseRgn = alloc_mouseRgns();
	
	make_main_window();
	window_size_changed( get_portRect( main_window ) );
	
	const bool has_WNE = has_WaitNextEvent();
	
	while ( ! quitting )
	{
		EventRecord event;
		
		if ( has_WNE ? WaitNextEvent( event ) : wait_next_event( event ) )
		{
			WindowRef window;
			
			switch ( event.what )
			{
				case mouseDown:
					if ( is_fullscreen )
					{
						click( event.where );
						break;
					}
					
					switch ( FindWindow( event.where, &window ) )
					{
						case inMenuBar:
							menu_item_chosen( MenuSelect( event.where ) );
							break;
						
					#if CONFIG_DESK_ACCESSORIES
						
						case inSysWindow:
							SystemClick( &event, window );
							break;
						
					#endif
						
						case inDrag:
							DragWindow( window, event.where, wide_drag_area() );
							calibrate_mouseRgns();
							break;
						
						case inGoAway:
							if ( TrackGoAway( window, event.where ) )
							{
								DisposeWindow( main_window );
								
								main_window = NULL;
								quitting = true;
							}
							break;
						
						case inContent:
							if ( CONFIG_DAs  &&  window != FrontWindow() )
							{
								SelectWindow( window );
								break;
							}
							
							GlobalToLocal( &event.where );
							click( event.where );
							break;
						
						case inGrow:
							if ( long grew = GrowWindow( window, event.where, &grow_size ) )
							{
								SizeWindow( window, grew, grew >> 16, true );
								
								invalidate_window_size( window );
							}
							break;
						
						default:
							break;
					}
					break;
				
				case keyDown:
					if ( event.modifiers & cmdKey )
					{
						menu_item_chosen( MenuKey( event.message ) );
						break;
					}
					
					if ( (char) event.message == 0x1B  &&  is_fullscreen )
					{
						// User pressed Esc
						
						is_fullscreen = false;
						
						leave_fullscreen();
						
						CheckMenuItem( Options_menu, Fullscreen, false );
					}
					
					break;
				
				case updateEvt:
					window = (WindowRef) event.message;
					
					SetPortWindowPort( window );
					
					BeginUpdate( window );
					draw_window( window );
					EndUpdate  ( window );
					break;
				
				case osEvt:
					switch ( (event.message & osEvtMessageMask) >> 24 )
					{
						case mouseMovedMessage:
							gMouseRgn = mouse_moved( event.where );
							break;
						
						default:
							break;
					}
					break;
				
			#if CONFIG_APPLE_EVENTS
				
				case kHighLevelEvent:
					(void) AEProcessAppleEvent( &event );
					break;
				
			#endif
				
				default:
					break;
			}
		}
	}
	
	if ( is_fullscreen )
	{
		cleanup_fullscreen();
	}
	
	return 0;
}
