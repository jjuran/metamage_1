/*
	Chess.cc
	--------
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
#include "mac_qd/globals/screenBits.hh"
#include "mac_qd/set_cursor_by_id.hh"
#include "mac_qd/wide_drag_area.hh"

// mac-app-utils
#include "mac_app/about_box.hh"
#include "mac_app/DAs.hh"
#include "mac_app/event_handlers.hh"
#include "mac_app/init.hh"
#include "mac_app/menus.hh"
#include "mac_app/state.hh"

// chess-logic
#include "chess/game.hh"

// Chess
#include "board.hh"
#include "graphics.hh"
#include "menus.hh"
#include "offscreen.hh"
#include "state.hh"
#include "window.hh"


#pragma exceptions off


#define CONFIG_DAs CONFIG_DESK_ACCESSORIES

#if TARGET_API_MAC_CARBON
#define SystemTask()  /**/
#endif

using mac::qd::wide_drag_area;


// gestaltAppleEventsAttr = 'evnt'

const bool apple_events_present =
	CONFIG_APPLE_EVENTS  &&
		(CONFIG_APPLE_EVENTS_GRANTED  ||
			mac::sys::gestalt( 'evnt' ) != 0);

static RgnHandle gBoardRgn = OPAQUE_TOOLBOX_STRUCTS ? NewRgn() : NULL;
static RgnHandle gInertRgn = NewRgn();
static RgnHandle gMouseRgn;

static inline
Point get_global_mouse()
{
#if ! TARGET_API_MAC_CARBON
	
	const Point& Mouse = *(Point*) 0x0830;
	
	return Mouse;
	
#endif
	
	Point pt;
	
	GetGlobalMouse( &pt );
	
	return pt;
}

static inline
bool point_in_rect_rgn( Point pt, RgnHandle rgn )
{
#if ! OPAQUE_TOOLBOX_STRUCTS
	
	return PtInRect( pt, &rgn[0]->rgnBBox );
	
#endif
	
	return PtInRgn( pt, rgn );
	
}

static
void set_regions()
{
#if ! OPAQUE_TOOLBOX_STRUCTS
	
	WindowPeek w = (WindowPeek) main_window;
	
	gBoardRgn = w->contRgn;
	
#endif
	
	if ( OPAQUE_TOOLBOX_STRUCTS )
	{
		GetWindowRegion( main_window, kWindowContentRgn, gBoardRgn );
	}
	
	RectRgn( gInertRgn, &mac::qd::screenBits().bounds );
	
	DiffRgn( gInertRgn, gBoardRgn, gInertRgn );
}

static
void mouse_moved( Point where )
{
	gMouseRgn = NULL;
	
	if ( ! chess::game.finished  &&  FrontWindow() == main_window )
	{
		if ( point_in_rect_rgn( where, gBoardRgn ) )
		{
			gMouseRgn = gBoardRgn;
			
			mac::qd::set_cursor_by_id( 128 + chess::game.mid_move );
			
			return;
		}
		else
		{
			gMouseRgn = gInertRgn;
		}
	}
	
	InitCursor();
}

static inline
void draw_window( WindowRef window )
{
	blit( (GrafPtr) GetWindowPort( window ) );
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
				const OSType creator = 'Kngt';
				
				mac::app::show_About_box( creator );
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
					reset();
					
					/*
						If the cursor is within the board,
						force a mouse-moved event in order to
						set the cursor, in case it had been
						black's turn (since it's now white's).
						
					*/
					
					gMouseRgn = gInertRgn;
					break;
				
				case Open:
					break;
				
				case Save:
				case SaveAs:
					break;
				
				case Close:
					if ( CONFIG_DAs  &&  mac::app::close_front_DA() )
					{
						break;
					}
					else if ( WindowRef window = FrontWindow() )
					{
						if ( mac::app::close_About_box( window ) )
						{
							break;
						}
					}
					// fall through
				
				case Quit:
					mac::app::quitting = true;
					break;
				
				default:
					break;
			}
		
		case Edit:
			break;
		
		default:
			break;
	}
	
	HiliteMenu( 0 );
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
	
	return GetNextEvent( everyEvent, &event );
}

int main()
{
	using mac::app::quitting;
	
	mac::app::init_toolbox();
	mac::app::install_menus();
	
	if ( apple_events_present )
	{
		mac::app::install_basic_event_handlers();
	}
	
	short length = make_main_window();
	
	set_offscreen_chess_board_size( length );
	
	unit_length = length / 8u;
	
	set_regions();
	
	reset();
	
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
							
							if ( window == main_window )
							{
								set_regions();
							}
							break;
						
						case inGoAway:
							if ( TrackGoAway( window, event.where ) )
							{
								if ( ! mac::app::close_About_box( window ) )
								{
									quitting = true;
								}
							}
							break;
						
						case inContent:
							if ( window != FrontWindow() )
							{
								SelectWindow( window );
							}
							else if ( window == main_window )
							{
								if ( ! chess::game.finished )
								{
									click_board( event.where );
									
									/*
										Simulate a mouse-moved event in order
										to set the cursor, in case it's the
										other player's turn now.
									*/
									
									mouse_moved( get_global_mouse() );
								}
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
					
					break;
				
				case activateEvt:
					window = (WindowRef) event.message;
					
					if ( window == main_window )
					{
						if ( event.modifiers & activeFlag )
						{
							SetPortWindowPort( window );
						}
						
						mouse_moved( event.where );
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
							mouse_moved( event.where );
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
	
	return 0;
}
