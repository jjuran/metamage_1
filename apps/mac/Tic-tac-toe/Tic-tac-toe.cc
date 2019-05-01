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

// mac-snd-utils
#include "mac_snd/playback.hh"

// mac-qd-utils
#include "mac_qd/get_portRect.hh"
#include "mac_qd/main_display_bounds.hh"
#include "mac_qd/wide_drag_area.hh"
#include "mac_qd/globals/arrow.hh"

// mac-app-utils
#include "mac_app/DAs.hh"
#include "mac_app/event_handlers.hh"
#include "mac_app/init.hh"
#include "mac_app/menus.hh"
#include "mac_app/state.hh"

// Tic-tac-toe
#include "cursors.hh"
#include "regions.hh"
#include "state.hh"


#define CONFIG_DAs CONFIG_DESK_ACCESSORIES

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

static player_t current_player = tictactoe::Player_X;

static inline
short min( short a, short b )
{
	return b < a ? b : a;
}

static
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

static RgnHandle mouseRgns[ 10 ];
static RgnHandle otherRgn;
static RgnHandle gMouseRgn;

static
void alloc_mouseRgns()
{
	for ( short i = 0;  i < 10;  ++i )
	{
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

static
short window_unitLength( WindowRef window )
{
	const Rect& portRect = get_portRect( window );
	
	const short portWidth  = portRect.right - portRect.left;
	const short portHeight = portRect.bottom - portRect.top;
	
	const short portLength = min( portWidth, portHeight );
	
	const short unitLength = portLength / 32;
	
	return unitLength;
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
	
	height -= height /  4;  // leave some space
	height -= height % 16;  // make it an exact multiple
	
	const short margin = usableHeight - height;
	
	bounds.top   += margin / 2;
	bounds.bottom = bounds.top + height;
	
	bounds.left  = (bounds.right - bounds.left - height) / 2;
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
	SetPortWindowPort( window );
	
	const Rect& portRect = get_portRect( window );
	
	EraseRect( &portRect );
	
	const short unitLength = window_unitLength( window );
	
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
		const short top = unitLength * (4 + 9 * i);
		
		for ( short j = 0;  j < 3;  ++j )
		{
			const short left = unitLength * (4 + 9 * j);
			
			if ( player_t token = tictactoe::get( i3 + j ) )
			{
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
void draw_token( WindowRef window, player_t token, short index )
{
	const short unitLength = window_unitLength( window );
	
	const short i = index / 3;
	const short j = index % 3;
	
	const short top = unitLength * (4 + 9 * i);
	const short left = unitLength * (4 + 9 * j);
	
	SetOrigin( -left, -top );
	
	RgnHandle rgn = token > 0 ? get_X_token( unitLength )
	                          : get_O_token( unitLength );
	
	PaintRgn( rgn );
	
	SetOrigin( 0, 0 );
}

static
short hit_test( WindowRef window, Point where )
{
	const short unitLength = window_unitLength( window );
	
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
void play_tone( UInt16 swCount )
{
	const UInt16 n_notes = 1;
	
	UInt16 buffer[ 1 + 3 * (n_notes + 1) ];
	
	UInt16* p = buffer;
	
	*p++ = swMode;
	
	const UInt16 amplitude = 30;  // 0 - 255
	const UInt16 duration  =  6;  // 0 - 65535
	
	*p++ = swCount;
	*p++ = amplitude;
	*p++ = duration;
	
	*p++ = 0;
	*p++ = 0;
	*p++ = 0;
	
	const SWSynthRec& rec = *(SWSynthRec*) buffer;
	
	mac::snd::play( rec, sizeof buffer );
}

static
void click( WindowRef window, Point where )
{
	if ( ! current_player )
	{
		return;
	}
	
	using namespace tictactoe;
	
	short i = hit_test( window, where );
	
	move_t result = move( current_player, i );
	
	if ( result < Move_ok )
	{
		return;
	}
	
	SetCursor( &mac::qd::arrow() );
	
	RgnHandle rgn = mouseRgns[ 1 + i ];
	
	gMouseRgn = mouseRgns[ 1 + i ] = otherRgn;
	
	XorRgn( otherRgn, rgn, otherRgn );
	
	DisposeRgn( rgn );
	
	draw_token( window, current_player, i );
	
	if ( sound_enabled )
	{
		const UInt16 tone = current_player == tictactoe::Player_X ? 629 : 1497;
		
		play_tone( tone );
	}
	
	if ( result == Move_ok )
	{
		current_player = opponent( current_player );
	}
	else if ( result > Move_ok )
	{
		current_player = Player_none;
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
	Point globalOffset = { 0, 0 };
	
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
			
			RgnHandle rgn = mouseRgns[ 1 + i3 + j ];
			
			if ( rgn != otherRgn )
			{
				RectRgn( rgn, &rect );
				
				XorRgn( otherRgn, rgn, otherRgn );
			}
		}
	}
}

static
void reset()
{
	tictactoe::reset();
	
	current_player = tictactoe::Player_X;
	
	for ( short i = 1;  i < 10;  ++i )
	{
		if ( mouseRgns[ i ] == otherRgn )
		{
			mouseRgns[ i ] = NewRgn();
		}
	}
	
	short unitLength = window_unitLength( main_window );
	
	calibrate_mouseRgns( unitLength );
	
	Point mouse;
	GetMouse( &mouse );
	
	short i = hit_test( main_window, mouse );
	
	SetCursor( i + 1 ? &X_cursor : &mac::qd::arrow() );
	
	gMouseRgn = mouseRgns[ i + 1 ];
	
	draw_window( main_window );
}

static
void menu_item_chosen( long choice )
{
	short menu = choice >> 16;
	short item = choice;
	
	switch ( menu )
	{
		case 1:  // (Apple)
			if ( item == 1 )
			{
				// About...
			}
			else if ( ! TARGET_API_MAC_CARBON )
			{
				mac::app::open_DA_from_menu( item );
			}
			break;
		
		case 2:  // File
			switch ( item )
			{
				case 1:  // New Game
					reset();
					break;
				
				case 2:  // Open
					break;
				
				case 3:  // Close
					if ( CONFIG_DAs  &&  mac::app::close_front_DA() )
					{
						break;
					}
				
				case 4:  // -
				case 5:  // Quit
					mac::app::quitting = true;
					break;
				
				default:
					break;
			}
		
		case 3:  // Edit
			break;
		
		case 4:  // Options
			sound_enabled = ! sound_enabled;
			
			CheckMenuItem( GetMenuHandle( menu ), item, sound_enabled );
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
	
	const short i = hit_test( main_window, where );
	
	const Cursor* cursor;
	
	if ( current_player  &&  tictactoe::can_move( i ) )
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
	MenuRef Options = GetMenuHandle( 4 );
	short   Sound   = 1;
	
#if ! TARGET_API_MAC_CARBON
	
	if ( SdVolume > 0 )
	{
		sound_enabled = true;
		
		CheckMenuItem( Options, Sound, sound_enabled );
	}
	else if ( ! SdEnable )
	{
		DisableItem( Options, Sound );
	}
	
	return;
	
#endif
	
	DisableMenuItem( Options, Sound );
}

int main()
{
	using mac::app::quitting;
	
	mac::app::init_toolbox();
	mac::app::install_menus();
	
	set_up_Options_menu();
	
	if ( apple_events_present )
	{
		mac::app::install_basic_event_handlers();
	}
	
	alloc_mouseRgns();
	
	make_main_window();
	
	short unitLength = window_unitLength( main_window );
	
	calculate_token_regions( unitLength );
	
	calibrate_mouseRgns( unitLength );
	
	while ( ! quitting )
	{
		EventRecord event;
		
		if ( WaitNextEvent( everyEvent, &event, 0x7FFFFFFF, gMouseRgn ) )
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
							click( window, event.where );
							break;
						
						case inGrow:
							if ( long grew = GrowWindow( window, event.where, &grow_size ) )
							{
								SizeWindow( window, grew, grew >> 16, true );
								
								InvalRect( window, get_portRect( window ) );
								
								unitLength = window_unitLength( window );
								
								calculate_token_regions( unitLength );
								
								calibrate_mouseRgns( unitLength );
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
					}
					
					break;
				
				case updateEvt:
					window = (WindowRef) event.message;
					
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
	
	return 0;
}
