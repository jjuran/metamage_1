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
#include "mac_ui/windows.hh"

// mac-app-utils
#include "mac_app/DAs.hh"
#include "mac_app/documents.hh"
#include "mac_app/event_handlers.hh"
#include "mac_app/init.hh"
#include "mac_app/menus.hh"
#include "mac_app/state.hh"

// Tic-tac-toe
#include "file_open.hh"
#include "file_save.hh"
#include "fullscreen.hh"
#include "menus.hh"
#include "regions.hh"
#include "state.hh"
#include "window.hh"
#include "window_events.hh"


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

static const Rect grow_size =
{
	50, 50,
	32767, 32767,
};

static RgnHandle gInertRgn;
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
	
#if TARGET_API_MAC_CARBON
	
	const bool osx_native = mac::sys::gestalt( 'sysv' ) >= 0x1000;
	
	const WindowAttributes attrs = kWindowCloseBoxAttribute
	                             | kWindowCollapseBoxAttribute
	                             | kWindowResizableAttribute
	                             | kWindowLiveResizeAttribute * osx_native
	                           #if CONFIG_COMPOSITING
	                             | kWindowCompositingAttribute
	                           #endif
	                           #ifdef MAC_OS_X_VERSION_10_3
	                             | kWindowAsyncDragAttribute
	                           #endif
	                           #ifdef MAC_OS_X_VERSION_10_7
	                             | kWindowHighResolutionCapableAttribute
	                           #endif
	                             ;
	
	OSStatus err;
	err = CreateNewWindow( kDocumentWindowClass, attrs, &bounds, &main_window );
	
	if ( err )
	{
		ExitToShell();
	}
	
	SetWTitle( main_window, "\p" "Tic-tac-toe" );
	
	install_window_event_handlers( main_window );
	
	ShowWindow( main_window );
	
#else
	
	main_window = NewWindow( NULL,
	                         &bounds,
	                         "\p" "Tic-tac-toe",
	                         true,
	                         documentProc,
	                         (WindowRef) -1,
	                         true,
	                         0 );
	
#endif
	
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
	
	window_size_changed( portRect );
	
	mac::ui::invalidate_window( window );
}

static WindowRef overlay;

static
void enter_fullscreen()
{
	fullscreen::enter();
	
	const Rect& portRect = main_display_bounds();
	
	window_size_changed( portRect );
	
#if CONFIG_COMPOSITING
	
	/*
		QuickTime's fullscreen blanking window isn't created with
		kWindowHighResolutionCapableAttribute, and it can't be set
		after the fact, so we need to create a new window if we might
		have a high resolution display.
		
		In addition, using kEventControlDraw avoids some unsightly
		flashing as the new window appears, so create a new window
		whenever we can use compositing, not just for high-res.
	*/
	
	const WindowAttributes attrs = kWindowCompositingAttribute
	                           #ifdef MAC_OS_X_VERSION_10_7
	                             | kWindowHighResolutionCapableAttribute
	                           #endif
	                             ;
	
	OSStatus err;
	err = CreateNewWindow( kSimpleWindowClass, attrs, &portRect, &overlay );
	
	install_window_event_handlers( overlay );
	
	SetPortWindowPort( overlay );
	
	ShowWindow( overlay );
	
	return;
	
#endif
	
	ForeColor( whiteColor );
	BackColor( blackColor );
	
	draw_window( portRect );
}

static
void leave_fullscreen()
{
	if ( CONFIG_COMPOSITING )
	{
		DisposeWindow( overlay );
		
		overlay = NULL;
	}
	
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
					InitCursor();
					file_open();
					gMouseRgn = gInertRgn;
					break;
				
				case Save:
				case SaveAs:
					InitCursor();
					file_save();
					gMouseRgn = gInertRgn;
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
					if ( int rgn_index = tictactoe::undo_move() + 1 )
					{
						RgnHandle rgn = reactivate_region( rgn_index );
						
						if ( CONFIG_COMPOSITING )
						{
							WindowRef window = is_fullscreen ? overlay
							                                 : main_window;
							
							mac::ui::invalidate_window( window );
						}
						else
						{
							erase_token_in_region( rgn );
						}
						
						/*
							Elicit a mouse-moved event to change the cursor if
							it's in a live square (outside of the dead zone).
						*/
						
						gMouseRgn = gInertRgn;
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
	using mac::app::open_documents_with;
	using mac::app::quitting;
	
	mac::app::init_toolbox();
	mac::app::install_menus();
	
	set_up_menus();
	set_up_Options_menu();
	
	if ( apple_events_present )
	{
		mac::app::install_basic_event_handlers();
	}
	
	gInertRgn =
	gMouseRgn = alloc_mouseRgns();
	
	make_main_window();
	window_size_changed( get_portRect( main_window ) );
	
	/*
		A new non-compositing window already has a full update region
		and doesn't need to be invalidated.
	*/
	
	if ( CONFIG_COMPOSITING )
	{
		mac::ui::invalidate_window( main_window );
	}
	
	propagate_to_dock_tile();
	
	open_launched_documents();
	
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
	
#ifdef __APPLE__
	
	RestoreApplicationDockTileImage();
	
#endif
	
	if ( is_fullscreen )
	{
		cleanup_fullscreen();
	}
	
	return 0;
}
