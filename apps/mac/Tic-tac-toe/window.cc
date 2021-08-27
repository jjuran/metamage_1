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
#include "mac_qd/globals/thePort.hh"

// mac-ui-utils
#include "mac_ui/menus.hh"

// CGTictactoe
#include "CGTictactoe.hh"

// Tic-tac-toe
#include "cursors.hh"
#include "dock_tile.hh"
#include "fullscreen_QT.hh"
#include "menus.hh"
#include "play_tone.hh"
#include "regions.hh"
#include "state.hh"


#ifdef MAC_OS_X_VERSION_10_0
#define CONFIG_USE_COREGRAPHICS  1
#else
#define CONFIG_USE_COREGRAPHICS  0
#endif

using mac::qd::get_portRect;

using tictactoe::move_t;
using tictactoe::player_t;

static player_t current_player = tictactoe::Player_X;
static player_t winning_player = tictactoe::Player_none;

static inline
short min( short a, short b )
{
	return b < a ? b : a;
}

static RgnHandle allocRgns[ 10 ];
static RgnHandle mouseRgns[ 10 ];
static RgnHandle otherRgn;

RgnHandle alloc_mouseRgns()
{
	for ( short i = 0;  i < 10;  ++i )
	{
		allocRgns[ i ] =
		mouseRgns[ i ] = NewRgn();
	}
	
	return otherRgn = mouseRgns[ 0 ];
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
RgnHandle play( short i, bool sound_enabled )
{
	if ( winning_player )
	{
		return NULL;
	}
	
	using namespace tictactoe;
	
	move_t result = move( current_player, i );
	
	if ( result < Move_ok )
	{
		return NULL;
	}
	
	SetCursor( &mac::qd::arrow() );
	
	RgnHandle rgn = mouseRgns[ 1 + i ];
	
	mouseRgns[ 1 + i ] = otherRgn;
	
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
	
	return otherRgn;
}

RgnHandle click( Point where, bool sound_enabled )
{
	return play( hit_test( where ), sound_enabled );
}

RgnHandle undo()
{
	int undone_index = tictactoe::undo_move();
	
	if ( undone_index < 0 )
	{
		return NULL;
	}
	
	int rgn_index = undone_index + 1;
	
	Point origin = {};
	GlobalToLocal( &origin );
	
	SetOrigin( -origin.h, -origin.v );
	
	RgnHandle rgn = mouseRgns[ rgn_index ] = allocRgns[ rgn_index ];
	
	EraseRgn( rgn );
	
	SetOrigin( 0, 0 );
	
	XorRgn( otherRgn, rgn, otherRgn );
	
	winning_player = tictactoe::Player_none;
	current_player = opponent( current_player );
	
	/*
		Elicit a mouse-moved event to change the cursor if it's in a live
		square (outside of the dead zone).
	*/
	
	return otherRgn;
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

void window_size_changed( const Rect& portRect )
{
	calculate_window_metrics( portRect );
	
	calculate_token_regions( unitLength );
	
	calibrate_mouseRgns();
}

RgnHandle reset()
{
	tictactoe::reset();
	
	current_player = tictactoe::Player_X;
	winning_player = tictactoe::Player_none;
	
	for ( short i = 1;  i < 10;  ++i )
	{
		mouseRgns[ i ] = allocRgns[ i ];
	}
	
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

void reload( const unsigned char* data, unsigned short size )
{
	const bool sound_enabled = false;
	
	reset();
	
	while ( size-- > 0 )
	{
		unsigned i = tictactoe::decode_cell_index( *data++ );
		
		play( i, sound_enabled );
	}
	
	propagate_to_dock_tile();
	
	if ( tictactoe::can_undo() )
	{
		mac::ui::enable_menu_item( Edit_menu, Undo );
	}
}
