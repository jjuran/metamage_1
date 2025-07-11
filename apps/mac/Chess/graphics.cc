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

// mac-sys-utils
#include "mac_sys/gestalt.hh"

// mac-qd-utils
#include "mac_qd/get_portRect.hh"
#include "mac_qd/globals/patterns.hh"
#include "mac_qd/globals/thePort.hh"
#include "mac_qd/plot_icon_id.hh"

// chess-logic
#include "chess/game.hh"

// Chess
#include "icons.hh"
#include "ideal.hh"
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

static const Pattern ltGray_14_bytes_ok =
{
	0x11, 0x44,
	0x11, 0x44,
	0x11, 0x44,
	0x11, 0x44,
};

static const Pattern ltGray_14_bytes_41 =
{
	0x44, 0x11,
	0x44, 0x11,
	0x44, 0x11,
	0x44, 0x11,
};

static const Pattern dkGray_7D_bytes_D7 =
{
	0xDD, 0x77,
	0xDD, 0x77,
	0xDD, 0x77,
	0xDD, 0x77,
};

static const Pattern dkGray_BE_bytes =
{
	0xBB, 0xEE,
	0xBB, 0xEE,
	0xBB, 0xEE,
	0xBB, 0xEE,
};

static inline
bool has_buggy_pattern_origin()
{
	if ( TARGET_API_MAC_CARBON )
	{
		static bool buggy_origin = mac::sys::gestalt( 'sysv' ) >= 0x1040;
		
		return buggy_origin;
	}
	
	return false;
}

#define buggy_origin has_buggy_pattern_origin()

#define ltGray_14_bytes (buggy_origin ? ltGray_14_bytes_41 : ltGray_14_bytes_ok)
#define dkGray_7D_bytes (buggy_origin ? dkGray_7D_bytes_D7 : mac::qd::dkGray())

void draw_square( int square, Layers layers, bool lit )
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
		
		const Pattern& gray = dark ? col & 1 ? dkGray_7D_bytes
		                                     : dkGray_BE_bytes
		                           : col & 1 ? ltGray_28_bytes
		                                     : ltGray_14_bytes;
		
		FillRect( &rect, &gray );
		
		if ( lit )
		{
			const short size = ((unit_length >= ideal_2x_length) + 1) * 3;
			
			PenState state;
			
			GetPenState( &state );
			
			PenMode( patCopy );
			PenSize( size, size );
			
			PenPat( &mac::qd::gray() );
			
			FrameRect( &rect );
			
			SetPenState( &state );
		}
	}
	
	Unit unit = game.board.unit[ i ];
	
	bool promoting = square == game.promoting_pawn_target;
	
	if ( unit  ||  promoting )
	{
		using mac::qd::plot_icon_id;
		
		const short height = unit_length;
		
		const short size = ideal_icon_size( unit_length );
		
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
