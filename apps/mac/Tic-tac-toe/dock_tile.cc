/*
	dock_tile.cc
	------------
*/

#include "dock_tile.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#endif


void propagate_to_dock_tile( short unitLength, Point margin )
{
#ifdef __APPLE__
	
	CGrafPtr gameboard_port = GetQDGlobalsThePort();
	CGrafPtr dock_tile_port = BeginQDContextForApplicationDockTile();
	
	Rect src_rect, dst_rect;
	
	src_rect.top  = margin.v;
	src_rect.left = margin.h;
	src_rect.bottom = margin.v + unitLength * 32;
	src_rect.right  = margin.h + unitLength * 32;
	
	GetPortBounds( dock_tile_port, &dst_rect );
	
	CopyBits( GetPortBitMapForCopyBits( gameboard_port ),
	          GetPortBitMapForCopyBits( dock_tile_port ),
	          &src_rect,
	          &dst_rect,
	          srcCopy,
	          NULL );
	
	// This is needed in 10.2 and 10.4 PPC, not in 10.5 x86
	QDFlushPortBuffer( dock_tile_port, NULL );
	
	EndQDContextForApplicationDockTile( dock_tile_port );
	
	SetPort( gameboard_port );
	
#endif
}
