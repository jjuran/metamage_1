/*
	window.cc
	---------
*/

#include "window.hh"

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#endif

// mac-qd-utils
#include "mac_qd/get_portRect.hh"
#include "mac_qd/globals/arrow.hh"
#include "mac_qd/globals/thePort_window.hh"

// mac-ui-utils
#include "mac_ui/menus.hh"
#include "mac_ui/windows.hh"

// CGTictactoe
#include "CGTictactoe.hh"

// Tic-tac-toe
#include "cursors.hh"
#include "dock_tile.hh"
#include "fullscreen_QT.hh"
#include "menus.hh"
#include "play_tone.hh"
#include "regions.hh"
#include "sound.hh"
#include "state.hh"


#ifdef MAC_OS_X_VERSION_10_0
#define CONFIG_USE_COREGRAPHICS  1
#else
#define CONFIG_USE_COREGRAPHICS  0
#endif

using mac::qd::get_portRect;

using tictactoe::move_t;
using tictactoe::player_t;

using tictactoe::current_player;
using tictactoe::winning_player;

static inline
short min( short a, short b )
{
	return b < a ? b : a;
}

static short unitLength;
static Point margin;

static
void calculate_window_metrics( const Rect& portRect )
{
	const short portWidth  = portRect.right - portRect.left;
	const short portHeight = portRect.bottom - portRect.top;
	
	const short portLength = min( portWidth, portHeight );
	
	unitLength = portLength / 32u;
	
	if ( unitLength == 0 )
	{
		unitLength = 1;
		
		margin.v = 0;
		margin.h = 0;
		
		return;
	}
	
	margin.v = (portHeight - unitLength * 32) / 2u;
	margin.h = (portWidth  - unitLength * 32) / 2u;
}

void propagate_to_dock_tile()
{
#ifdef __APPLE__
	
	draw_dock_tile( tictactoe::squares );
	
#endif
}

class CGContextForPort
{
	private:
		CGContextRef context;
		
		// non-copyable
		CGContextForPort           ( const CGContextForPort& );
		CGContextForPort& operator=( const CGContextForPort& );
	
	public:
		CGContextForPort( bool erasing = false );
		
		~CGContextForPort();
		
		operator CGContextRef() const  { return context; }
};

CGContextForPort::CGContextForPort( bool erasing )
{
	const bool fullscreen = is_fullscreen_via_QT();
	
	float white_or_black = fullscreen;
	
	CGrafPtr port = GetQDGlobalsThePort();
	
	CreateCGContextForPort( port, &context );
	
	const Rect& portRect = get_portRect( port );
	
	if ( erasing )
	{
		CGRect whole = {};
		
		whole.size.width  = portRect.right;
		whole.size.height = portRect.bottom;
		
		CGContextSetGrayFillColor( context, ! fullscreen, 1 );
		
		CGContextFillRect( context, whole );
	}
	
	/*
		Flip the coordinate system so that the Y axis increases,
		downward as in QuickDraw (and compositing-mode windows).
	*/
	
	CGContextTranslateCTM( context, 0, portRect.bottom );
	CGContextScaleCTM    ( context, 1, -1 );
	
	CGContextTranslateCTM( context, margin.h, margin.v );
	CGContextScaleCTM( context, unitLength, unitLength );
	
	CGContextSetGrayStrokeColor( context, white_or_black, 1 );
}

CGContextForPort::~CGContextForPort()
{
	CGContextFlush  ( context );  // required in Mac OS X 10.4
	CGContextRelease( context );
}

void draw_window( CGContextRef context )
{
	// Caller saves and restores the graphics context as needed.
	
	const bool fullscreen = is_fullscreen_via_QT();
	
	const float fullscreen_white_or_window_black =   fullscreen;
	const float fullscreen_black_or_window_white = ! fullscreen;
	
	CGContextTranslateCTM( context, margin.h, margin.v );
	CGContextScaleCTM( context, unitLength, unitLength );
	
	CGContextSetGrayStrokeColor( context, fullscreen_white_or_window_black, 1 );
	CGContextSetGrayFillColor  ( context, fullscreen_black_or_window_white, 1 );
	
#ifdef MAC_OS_X_VERSION_10_3
	
	/*
		CGContextGetClipBoundingBox() doesn't exist before 10.3,
		but this is okay because we only get here through the compositing
		path, which we only enable for 10.3+.
	*/
	
	CGContextFillRect( context, CGContextGetClipBoundingBox( context ) );
	
#endif
	
	draw_board( context, tictactoe::squares );
}

void draw_window( const Rect& portRect )
{
	if ( CONFIG_USE_COREGRAPHICS )
	{
		draw_board( CGContextForPort( true ), tictactoe::squares );
		return;
	}
	
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
void draw_token( player_t token, short index )
{
	const short i = index / 3;
	const short j = index % 3;
	
	if ( CONFIG_USE_COREGRAPHICS )
	{
		draw_token( CGContextForPort(), token, j, i );
		return;
	}
	
	const short top  = margin.v + unitLength * (4 + 9 * i);
	const short left = margin.h + unitLength * (4 + 9 * j);
	
	SetOrigin( -left, -top );
	
	RgnHandle rgn = token > 0 ? get_X_token( unitLength )
	                          : get_O_token( unitLength );
	
	PaintRgn( rgn );
	
	SetOrigin( 0, 0 );
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
RgnHandle play( short i )
{
	player_t playing_player = current_player;
	
	using namespace tictactoe;
	
	move_t result = move( i );
	
	if ( result < Move_ok )
	{
		return NULL;
	}
	
	SetCursor( &mac::qd::arrow() );
	
	draw_token( playing_player, i );
	
	if ( sound_enabled )
	{
		const UInt16 tone = tone_for_player( playing_player );
		
		play_tone( tone );
	}
	
	return deactivate_region( 1 + i );
}

RgnHandle click_board( Point where )
{
	return play( hit_test( where ) );
}

void erase_token_in_region( RgnHandle rgn )
{
	Point origin = {};
	GlobalToLocal( &origin );
	
	SetOrigin( -origin.h, -origin.v );
	
	EraseRgn( rgn );
	
	SetOrigin( 0, 0 );
}

static
void calculate_token_regions( short unitLength )
{
	get_X_token( unitLength );
	get_O_token( unitLength );
}

void calibrate_mouseRgns()
{
	Point globalOffset = margin;
	
	LocalToGlobal( &globalOffset );
	
	calibrate_mouseRgns( globalOffset, unitLength );
}

void window_size_changed( const Rect& portRect )
{
	calculate_window_metrics( portRect );
	
	calculate_token_regions( unitLength );
	
	calibrate_mouseRgns();
}

RgnHandle reset()
{
	tictactoe::reset();
	
	reset_mouseRgns();
	
	calibrate_mouseRgns();
	
	/*
		Elicit a mouse-moved event to change the cursor if it's in a live
		square (outside of the dead zone).
	*/
	
	draw_window( get_portRect( mac::qd::thePort() ) );
	
	return otherRgn;
}

RgnHandle mouse_moved( Point where )
{
	GlobalToLocal( &where );
	
	const short i = hit_test( where );
	
	const Cursor* cursor;
	
	if ( tictactoe::can_move( i ) )
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

void reload( const unsigned char* data, unsigned short size )
{
	while ( size-- > 0 )
	{
		unsigned i = tictactoe::decode_cell_index( *data++ );
		
		deactivate_region( 1 + i );
	}
	
	propagate_to_dock_tile();
	
	mac::ui::invalidate_window( mac::qd::thePort_window() );
	
	if ( tictactoe::can_undo() )
	{
		mac::ui::enable_menu_item( Edit_menu, Undo );
	}
}
