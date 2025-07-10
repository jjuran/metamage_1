/*
	board.cc
	--------
*/

#include "board.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#endif

// mac-glue-utils
#include "mac_glue/OSUtils.hh"

// mac-qd-utils
#include "mac_qd/copy_bits.hh"
#include "mac_qd/get_portRect.hh"
#include "mac_qd/plot_icon_id.hh"

// chess-logic
#include "chess/game.hh"
#include "chess/play.hh"
#include "chess/setup.hh"

// Chess
#include "graphics.hh"
#include "icons.hh"
#include "ideal.hh"
#include "offscreen.hh"
#include "state.hh"

#include "Rez/app_name.h"


#pragma exceptions off


using mac::qd::get_portRect;

using chess::Square;


bool live_dragging_enabled;

static Square& promotion_source = chess::game.promoting_pawn_source;
static Square& promotion_target = chess::game.promoting_pawn_target;

static
Square hit_test( Point where )
{
	short row = where.v / unit_length;
	short col = where.h / unit_length;
	
	if ( (unsigned short) row < 8  &&  (unsigned short) col < 8 )
	{
		int rank = 7 - row;
		int file =     col;
		
		return Square( rank * 8 + file + 1 );
	}
	
	return Square();
}

static
short quadrant_hit_test( Point where )
{
	short row_x2 = 2 * where.v / unit_length;
	short col_x2 = 2 * where.h / unit_length;
	
	return + (row_x2 & 0x1) << 1
	       | (col_x2 & 0x1);
}

static inline
void click_for_promotion( GrafPtr port, Point where )
{
	using namespace chess;
	
	short quadrant = quadrant_hit_test( where );
	
	Type promotion_type = Type( Type_knight + (~quadrant & 0x3) );
	
	if ( play( game, promotion_source, promotion_target, promotion_type ) )
	{
		SetPort( offscreen_port );
		
		draw_square( promotion_source, Layer_all );
		
		Square source = promotion_target;
		
		promotion_source = Square_none;
		promotion_target = Square_none;
		
		draw_square( source, Layer_all );
		
		if ( game.finished )
		{
			draw_square( game.army[ 0 ].addr[ Role_king ], Layer_all );
			draw_square( game.army[ 1 ].addr[ Role_king ], Layer_all );
		}
		
		blit( port );
	}
}

static
void blit_square( GrafPtr port, int square )
{
	int i = square - 1;
	
	int row = 7 - i / 8u;
	int col =     i % 8u;
	
	int x = col * unit_length;
	int y = row * unit_length;
	
	Rect rect = { y, x, y + unit_length, x + unit_length };
	
	mac::qd::copy_bits( offscreen_port, port, rect, rect );
}

static inline
Square drag_unit( GrafPtr port, Square source )
{
	using namespace chess;
	
	if ( live_dragging_enabled )
	{
		SetPort( offscreen_port );
	}
	
	draw_square( source, Layer_mask );
	
	SetPort( port );
	
	Type clicked_type = game.type_at( source );
	
	short icon_id = icon_ResID_for_type[ clicked_type ];
	
	int i = source - 1;
	
	short row = 7 - i / 8u;
	short col =     i % 8u;
	
	int x = col * unit_length;
	int y = row * unit_length;
	
	Handle h = GetResource( 'ICN#', icon_id );
	
	if ( ! h )
	{
		return Square();
	}
	
	UInt32 rows[ 32 * 2 ];
	
	BlockMoveData( *h, rows, sizeof rows );
	
	BitMap icon_bits = { (Ptr) rows, 4, { 0, 0, 32, 32 } };
	
	bool black = game.mid_move;
	
	short mode = black ? srcOr : srcBic;
	
	const short height = unit_length;
	
	const short size = ideal_icon_size( unit_length );
	
	Rect rect;
	
	rect.top  = y + (height - size    ) / 2;
	rect.left = x + (height - size + 1) / 2;
	
	rect.bottom = rect.top + size;
	rect.right = rect.left + size;
	
	Point where;
	
	where.v = rect.top  + size / 2;
	where.h = rect.left + size / 2;
	
	HideCursor();
	
	RgnHandle rgn;
	
	if ( ! live_dragging_enabled )
	{
		static RgnHandle a = NewRgn();
		static RgnHandle b = NewRgn();
		
		icon_bits.baseAddr += 128;
		
		BitMapToRegion( a, &icon_bits );
		
		MapRgn( a, &icon_bits.bounds, &rect );
		
		CopyRgn( a, b );
		
		InsetRgn( b, 2, 2 );
		
		DiffRgn( a, b, b );
		
		rgn = b;
		
		PenMode( patXor );
		
		PaintRgn( rgn );
	}
	
	const Rect& portRect = get_portRect( port );
	
	Square lit = Square();
	Square hit = Square();
	
	do
	{
		Point pt = where;
		
		GetMouse( &where );
		
		if ( *(long*) &where != *(long*) &pt )
		{
			if ( ! live_dragging_enabled )
			{
				PaintRgn( rgn );
			}
			
			OffsetRect( &rect, where.h - pt.h, where.v - pt.v );
			
			/*
				Each time the cursor moves, hit-test the location
				to see which square it's over and whether that's
				a legal move.  Highlight any current legal move.
			*/
			
			hit = PtInRect( where, &portRect ) ? hit_test( where ) : Square();
			
			if ( hit != lit )
			{
				/*
					There's a pattern origin bug in 10.4+ and later
					when draw_square() is drawing to the offscreen port,
					which it works around by selecting different patterns
					to compensate.  There's a *different* pattern origin
					bug (affecting squares in even-numbered ranks) when
					drawing to the window, which we'll sidestep here by
					drawing only to the offscreen port (and then blitting
					the results to the window).
					
					Unlike the previous workaround of switching patterns,
					this workaround is harmless when the bug is absent,
					so for simplicity we apply it in all Carbon builds.
					
				*/
				
				if ( TARGET_API_MAC_CARBON  ||  live_dragging_enabled )
				{
					SetPort( offscreen_port );
				}
				
				if ( lit )
				{
					/*
						A square that the cursor no longer points to
						has highlighting.  Redraw to remove it.
					*/
					
					draw_square( lit, Layer_all );
					
					if ( TARGET_API_MAC_CARBON  &&  ! live_dragging_enabled )
					{
						/*
							This is the workaround for the second pattern
							origin bug mentioned above.  Blit the redrawn
							square with highlight removed to the window.
						*/
						
						SetPort( port );
						
						blit_square( port, lit );
					}
					
					lit = Square();
				}
				
				Game hypothetical = game;
				
				if ( play( hypothetical, source, hit ) )
				{
					lit = hit;
					
					/*
						The square newly pointed to by the cursor is
						a legal move.  Redraw it with a highlight.
					*/
					
					if ( TARGET_API_MAC_CARBON )
					{
						/*
							If we're applying the workaround, then we
							switched to the window port above and need
							to switch back.  For live dragging, this is
							unnecessary (i.e. redundant) but harmless.
							
						*/
						
						SetPort( offscreen_port );
					}
					
					draw_square( lit, Layer_all, true );
					
					if ( TARGET_API_MAC_CARBON  &&  ! live_dragging_enabled )
					{
						/*
							As before, if we're applying the workaround,
							then blit the redrawn square to the window.
						*/
						
						SetPort( port );
						
						blit_square( port, lit );
					}
				}
				
				SetPort( port );
			}
			
			if ( live_dragging_enabled )
			{
				animate_icon( port, icon_bits, rect, mode );
			}
			else
			{
				OffsetRgn( rgn, where.h - pt.h, where.v - pt.v );
				
				PaintRgn( rgn );
			}
		}
		
		mac::glue::delay( 1 );
	}
	while ( StillDown() );
	
	if ( ! live_dragging_enabled )
	{
		PaintRgn( rgn );
		
		PenNormal();
	}
	
	ShowCursor();
	
	return PtInRect( where, &portRect ) ? hit_test( where ) : Square();
}

void click_board( Point where )
{
	using namespace chess;
	
	GlobalToLocal( &where );
	
	Square source = hit_test( where );
	
	if ( source <= 0 )
	{
		return;
	}
	
	GrafPtr window_port;
	
	GetPort( &window_port );
	
	if ( source == promotion_target )
	{
		click_for_promotion( window_port, where );
		
		return;
	}
	
	if ( promotion_target )
	{
		return;
	}
	
	Unit clicked_unit = game.board[ source ];
	
	if ( ! game.serving( clicked_unit ) )
	{
		return;
	}
	
	Square target = drag_unit( window_port, source );
	
	SetPort( offscreen_port );
	
	Layers source_layers = Layer_unit;
	
	if ( target > 0 )
	{
		const Army& current = game.current();
		
		Square rook_1_src = current.addr[ Role_rook_1 ];
		Square rook_2_src = current.addr[ Role_rook_2 ];
		
		Square en_passant = game.prev_en_passant_square;
		
		if ( target == en_passant )
		{
			en_passant = Square( en_passant - (game.mid_move ? -8 : 8) );
		}
		
		Playable playable = play( game, source, target );
		
		if ( playable < 0 )
		{
			promotion_source = source;
			promotion_target = target;
			
			source_layers = Layer_mask;
		}
		else if ( playable )
		{
			Role role = role_of_unit( game.board[ target ] );
			
			Type type = current.type[ role ];
			
			if ( type == Type_pawn  &&  en_passant )
			{
				draw_square( en_passant, Layer_all );
			}
			
			if ( role == Role_king )
			{
				Square rook_1_dst = current.addr[ Role_rook_1 ];
				Square rook_2_dst = current.addr[ Role_rook_2 ];
				
				if ( rook_1_dst  &&  rook_1_dst != rook_1_src )
				{
					draw_square( rook_1_src, Layer_all  );
					draw_square( rook_1_dst, Layer_unit );
				}
				else if ( rook_2_dst  &&  rook_2_dst != rook_2_src )
				{
					draw_square( rook_2_src, Layer_all  );
					draw_square( rook_2_dst, Layer_unit );
				}
			}
			
			if ( game.finished )
			{
				draw_square( game.army[ 0 ].addr[ Role_king ], Layer_all );
				draw_square( game.army[ 1 ].addr[ Role_king ], Layer_all );
			}
			
			source_layers = Layer_all;  // redraw starting square entirely
			
		}
		
		draw_square( target, Layer_all );
	}
	
	draw_square( source, source_layers );
	
	blit( window_port );
}

void reset()
{
	setup( chess::game );
	
	GrafPtr window_port;
	
	GetPort( &window_port );
	
	SetPort( offscreen_port );
	
	draw_board();
	
	blit( window_port );
}
