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
#include "mac_qd/globals/arrow.hh"

// mac-ui-utils
#include "mac_ui/menus.hh"

// mac-app-utils
#include "mac_app/DAs.hh"
#include "mac_app/event_handlers.hh"
#include "mac_app/init.hh"
#include "mac_app/menus.hh"
#include "mac_app/state.hh"

// Tic-tac-toe
#include "cursors.hh"
#include "dock_tile.hh"
#include "fullscreen.hh"
#include "fullscreen_port.hh"
#include "fullscreen_QT.hh"
#include "menus.hh"
#include "play_tone.hh"
#include "regions.hh"
#include "state.hh"


#define CONFIG_DAs CONFIG_DESK_ACCESSORIES

#if TARGET_API_MAC_CARBON
#define SystemTask()  /**/
#endif

using mac::qd::get_portRect;
using mac::qd::main_display_bounds;
using mac::qd::wide_drag_area;

using tictactoe::move_t;
using tictactoe::player_t;


// gestaltAppleEventsAttr = 'evnt'

const bool apple_events_present =
	CONFIG_APPLE_EVENTS  &&
		(CONFIG_APPLE_EVENTS_GRANTED  ||
			mac::sys::gestalt( 'evnt' ) != 0);

static bool sound_enabled;
static bool is_fullscreen;

static player_t current_player = tictactoe::Player_X;
static player_t winning_player = tictactoe::Player_none;

static inline
short min( short a, short b )
{
	return b < a ? b : a;
}

static inline
GrafPtr GetPort()
{
	GrafPtr port;
	GetPort( &port );
	
	return port;
}

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

static RgnHandle allocRgns[ 10 ];
static RgnHandle mouseRgns[ 10 ];
static RgnHandle otherRgn;
static RgnHandle gMouseRgn;

static
void alloc_mouseRgns()
{
	for ( short i = 0;  i < 10;  ++i )
	{
		allocRgns[ i ] =
		mouseRgns[ i ] = NewRgn();
	}
	
	gMouseRgn = otherRgn = mouseRgns[ 0 ];
}

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

static short unitLength;
static Point margin;

static
void calculate_window_metrics( const Rect& portRect )
{
	const short portWidth  = portRect.right - portRect.left;
	const short portHeight = portRect.bottom - portRect.top;
	
	const short portLength = min( portWidth, portHeight );
	
	unitLength = portLength / 32u;
	
	margin.v = (portHeight - portLength) / 2u;
	margin.h = (portWidth  - portLength) / 2u;
}

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

static inline
void propagate_to_dock_tile()
{
#ifdef __APPLE__
	
	if ( is_fullscreen_via_QT() )
	{
		return;
	}
	
	propagate_to_dock_tile( unitLength, margin );
	
#endif
}

static
void draw_window( const Rect& portRect )
{
	EraseRect( &portRect );
	
	SetOrigin( -margin.h, -margin.v );
	
	Rect line;
	
	line.left  = unitLength * 3;
	line.right = unitLength * 29;
	
	line.top    = unitLength * 11;
	line.bottom = line.top + unitLength;
	
	PaintRect( &line );
	
	line.top    = unitLength * 20;
	line.bottom = line.top + unitLength;
	
	PaintRect( &line );
	
	line.top    = unitLength * 3;
	line.bottom = unitLength * 29;
	
	line.left  = unitLength * 11;
	line.right = line.left + unitLength;
	
	PaintRect( &line );
	
	line.left  = unitLength * 20;
	line.right = line.left + unitLength;
	
	PaintRect( &line );
	
	const short width  = unitLength * 6;
	const short height = unitLength * 6;
	
	Rect rect = { 0, 0, height, width };
	
	for ( short i = 0, i3 = 0;  i < 3;  ++i, i3 += 3 )
	{
		const short top = margin.v + unitLength * (4 + 9 * i);
		
		for ( short j = 0;  j < 3;  ++j )
		{
			if ( player_t token = tictactoe::get( i3 + j ) )
			{
				const short left = margin.h + unitLength * (4 + 9 * j);
				
				SetOrigin( -left, -top );
				
				RgnHandle rgn = token > 0 ? get_X_token( unitLength )
				                          : get_O_token( unitLength );
				
				PaintRgn( rgn );
			}
		}
	}
	
	SetOrigin( 0, 0 );
}

static
void draw_window( WindowRef window )
{
	draw_window( get_portRect( window ) );
}

static
void draw_token( player_t token, short index )
{
	const short i = index / 3;
	const short j = index % 3;
	
	const short top  = margin.v + unitLength * (4 + 9 * i);
	const short left = margin.h + unitLength * (4 + 9 * j);
	
	SetOrigin( -left, -top );
	
	RgnHandle rgn = token > 0 ? get_X_token( unitLength )
	                          : get_O_token( unitLength );
	
	PaintRgn( rgn );
	
	SetOrigin( 0, 0 );
	
	propagate_to_dock_tile();
}

static
short hit_test( Point where )
{
	where.v -= margin.v;
	where.h -= margin.h;
	
	short x = where.h / unitLength;
	short y = where.v / unitLength;
	
	x -= 3;
	y -= 3;
	
	if ( (unsigned short) x > 25  ||  (unsigned short) y > 25 )
	{
		// hit in margin
		return -1;
	}
	
	if ( ++x % 9 == 0  ||  ++y % 9 == 0 )
	{
		// hit on a grid line
		return -1;
	}
	
	return y / 9 * 3 + x / 9;
}

static
void click( Point where )
{
	if ( winning_player )
	{
		return;
	}
	
	using namespace tictactoe;
	
	short i = hit_test( where );
	
	move_t result = move( current_player, i );
	
	if ( result < Move_ok )
	{
		return;
	}
	
	SetCursor( &mac::qd::arrow() );
	
	RgnHandle rgn = mouseRgns[ 1 + i ];
	
	gMouseRgn = mouseRgns[ 1 + i ] = otherRgn;
	
	XorRgn( otherRgn, rgn, otherRgn );
	
	draw_token( current_player, i );
	
	if ( sound_enabled )
	{
		const UInt16 tone = current_player == tictactoe::Player_X ? 629 : 1497;
		
		play_tone( tone );
	}
	
	if ( result > Move_ok )
	{
		winning_player = current_player;
	}
	
	current_player = opponent( current_player );
	
	mac::ui::enable_menu_item( Edit_menu, Undo );
}

static
void undo()
{
	int undone_index = tictactoe::undo_move();
	
	if ( undone_index < 0 )
	{
		return;
	}
	
	int rgn_index = undone_index + 1;
	
	Point origin = {};
	GlobalToLocal( &origin );
	
	SetOrigin( -origin.h, -origin.v );
	
	RgnHandle rgn = mouseRgns[ rgn_index ] = allocRgns[ rgn_index ];
	
	EraseRgn( rgn );
	
	SetOrigin( 0, 0 );
	
	propagate_to_dock_tile();
	
	XorRgn( otherRgn, rgn, otherRgn );
	
	winning_player = tictactoe::Player_none;
	current_player = opponent( current_player );
	
	/*
		Elicit a mouse-moved event to change the cursor if it's in a live
		square (outside of the dead zone).
	*/
	
	gMouseRgn = otherRgn;
	
	if ( ! tictactoe::can_undo() )
	{
		mac::ui::disable_menu_item( Edit_menu, Undo );
	}
}

static
void calculate_token_regions( short unitLength )
{
	get_X_token( unitLength );
	get_O_token( unitLength );
}

static
void calibrate_mouseRgns( short unitLength )
{
	Point globalOffset = margin;
	
	LocalToGlobal( &globalOffset );
	
	Rect rect = { -32000, -32000, 32000, 32000 };
	
	RectRgn( otherRgn, &rect );
	
	for ( short i = 0, i3 = 0;  i < 3;  ++i, i3 += 3 )
	{
		rect.top    = globalOffset.v + unitLength * (3 + 9 * i);
		rect.bottom = globalOffset.v + unitLength * (3 + 9 * i + 8);
		
		for ( short j = 0;  j < 3;  ++j )
		{
			rect.left  = globalOffset.h + unitLength * (3 + 9 * j);
			rect.right = globalOffset.h + unitLength * (3 + 9 * j + 8);
			
			RectRgn( allocRgns[ 1 + i3 + j ], &rect );
			
			RgnHandle rgn = mouseRgns[ 1 + i3 + j ];
			
			if ( rgn != otherRgn )
			{
				XorRgn( otherRgn, rgn, otherRgn );
			}
		}
	}
}

static
void window_size_changed( const Rect& portRect )
{
	calculate_window_metrics( portRect );
	
	calculate_token_regions( unitLength );
	
	calibrate_mouseRgns( unitLength );
}

static
void window_size_changed( WindowRef window )
{
	window_size_changed( get_portRect( window ) );
}

static inline
WindowRef GetPort_window()
{
	/*
		Return the window whose port is the current port.
		If accessor calls are functions, the port must belong to a window.
		Otherwise, it may be just a GrafPort.
	*/
	
	return GetWindowFromPort( (CGrafPtr) GetPort() );
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
	
	WindowRef window = GetPort_window();
	
	window_size_changed( window );
	
	draw_window( window );
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
void reset()
{
	tictactoe::reset();
	
	current_player = tictactoe::Player_X;
	winning_player = tictactoe::Player_none;
	
	for ( short i = 1;  i < 10;  ++i )
	{
		mouseRgns[ i ] = allocRgns[ i ];
	}
	
	calibrate_mouseRgns( unitLength );
	
	/*
		Elicit a mouse-moved event to change the cursor if it's in a live
		square (outside of the dead zone).
	*/
	
	gMouseRgn = otherRgn;
	
	draw_window( GetPort_window() );
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
					reset();
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
					undo();
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

static
RgnHandle mouse_moved( Point where )
{
	GlobalToLocal( &where );
	
	const short i = hit_test( where );
	
	const Cursor* cursor;
	
	if ( ! winning_player  &&  tictactoe::can_move( i ) )
	{
		cursor = current_player > 0 ? &X_cursor : &O_cursor;
	}
	else
	{
		cursor = &mac::qd::arrow();
	}
	
	SetCursor( cursor );
	
	return mouseRgns[ 1 + i ];
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
	
	alloc_mouseRgns();
	
	make_main_window();
	window_size_changed( main_window );
	
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
							calibrate_mouseRgns( unitLength );
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
