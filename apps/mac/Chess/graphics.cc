/*
	graphics.cc
	-----------
*/

#include "graphics.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif
#ifndef __LOWMEM__
#include <LowMem.h>
#endif

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#endif

// mac-qd-utils
#include "mac_qd/get_portRect.hh"
#include "mac_qd/globals/patterns.hh"
#include "mac_qd/globals/thePort.hh"
#include "mac_qd/plot_icon_id.hh"

// chess-logic
#include "chess/game.hh"

// Chess
#include "icons.hh"
#include "state.hh"


#pragma exceptions off


short unit_length;

static const Pattern ltGray_28_bytes =
{
	0x22, 0x88,
	0x22, 0x88,
	0x22, 0x88,
	0x22, 0x88,
};

static const Pattern ltGray_14_bytes =
{
	0x11, 0x44,
	0x11, 0x44,
	0x11, 0x44,
	0x11, 0x44,
};

static const Pattern dkGray_BE_bytes =
{
	0xBB, 0xEE,
	0xBB, 0xEE,
	0xBB, 0xEE,
	0xBB, 0xEE,
};

void draw_square( int square, Layers layers )
{
	using namespace chess;
	
	int i = square - 1;
	
	int row = 7 - i / 8u;
	int col =     i % 8u;
	
	int x = col * unit_length;
	int y = row * unit_length;
	
	Rect rect = { y, x, y + unit_length, x + unit_length };
	
	if ( layers < 0 )
	{
		int dark = (row ^ col) & 1;
		
		const Pattern& gray = dark ? col & 1 ? mac::qd::dkGray()  // 7D
		                                     : dkGray_BE_bytes
		                           : col & 1 ? ltGray_28_bytes
		                                     : ltGray_14_bytes;
		
		FillRect( &rect, &gray );
	}
	
	Unit unit = game.board.unit[ i ];
	
	bool promoting = square == game.promoting_pawn_target;
	
	if ( unit  ||  promoting )
	{
		using mac::qd::plot_icon_id;
		
		const short height = unit_length;
		
		const short size = 32;
		
		rect.top  += (height - size    ) / 2;
		rect.left += (height - size + 1) / 2;
		
		rect.bottom = rect.top + size;
		rect.right = rect.left + size;
		
		if ( promoting )
		{
			bool black = game.mid_move;
			
			if ( black )
			{
				ForeColor( whiteColor );
				BackColor( blackColor );
			}
			
			Rect nw = rect;
			Rect ne = rect;
			Rect sw = rect;
			Rect se = rect;
			
			nw.bottom -= size / 2u;
			ne.bottom -= size / 2u;
			sw.top    += size / 2u;
			se.top    += size / 2u;
			
			nw.right -= size / 2u;
			sw.right -= size / 2u;
			ne.left  += size / 2u;
			se.left  += size / 2u;
			
			using namespace chess;
			
			plot_icon_id( nw, icon_ResID_for_type[ Type_queen  ] );
			plot_icon_id( ne, icon_ResID_for_type[ Type_rook   ] );
			plot_icon_id( sw, icon_ResID_for_type[ Type_bishop ] );
			plot_icon_id( se, icon_ResID_for_type[ Type_knight ] );
			
			if ( black )
			{
				ForeColor( blackColor );
				BackColor( whiteColor );
			}
		}
		else if ( unit )
		{
			bool black = unit < 0;
			
			if ( black )
			{
				ForeColor( whiteColor );
				BackColor( blackColor );
			}
			
			if ( layers == Layer_mask )
			{
				ForeColor( black ? blackColor : whiteColor );
			}
			
			const Army& army = game.army[ unit < 0 ];
			
			Role role = role_of_unit( unit );
			
			Type type = army.type[ role ];
			
			short resID = icon_ResID_for_type[ type ];
			
			plot_icon_id( rect, resID );
			
			ForeColor( blackColor );
			BackColor( whiteColor );
		}
	}
}

void draw_board()
{
	for ( int i = 1;  i <= 64;  ++i )
	{
		draw_square( i, Layer_all );
	}
}
