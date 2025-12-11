/*
	BitMapToRegion.cc
	-----------------
*/

#include "BitMapToRegion.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// quickdraw
#include "qd/region_scanner.hh"

// ams-common
#include "callouts.hh"
#include "math.hh"


#pragma exceptions off


using quickdraw::region_scanner;


short MemErr : 0x0220;


static inline
short* rgn_extent( MacRegion* rgn )
{
	return (short*) &rgn[ 1 ];
}

pascal OSErr BitMapToRegion_patch( RgnHandle rgn, const BitMap* bitmap )
{
	const Rect& bounds = bitmap->bounds;
	
	if ( bounds.bottom <= bounds.top  ||  bounds.right <= bounds.left )
	{
		SetEmptyRgn( rgn );
		
		return noErr;
	}
	
	const short rowBytes = bitmap->rowBytes;
	
	const UInt16 height = bounds.bottom - bounds.top;
	const UInt16 width  = bounds.right - bounds.left;
	
	/*
		width and height are at least 1 and have a theoretical maximum
		of 65534 (32767 - -32767), so we can safely add 1 to each.
		
		If width is exactly 65534, then max_h_longs will be 32768,
		which is still small enough to fit in a UInt16 (which is why
		we're counting longwords instead of words or bytes).
	*/
	
	const UInt16 half_max_h_coords = width + 1 >> 1;
	
	const UInt16 max_h_longs = 1 + half_max_h_coords;
	const UInt32 max_rgn_size = 10 + mulu_w( max_h_longs, height + 1 ) * 4 + 2;
	
	SetHandleSize( (Handle) rgn, max_rgn_size + rowBytes );
	
	UInt16* temp_space = (UInt16*) *rgn + max_rgn_size / 2;
	
	if ( MemErr )
	{
		return MemErr;
	}
	
	int margin = -width & 0xF;
	
	short v = bounds.top;
	
	short* extent = rgn_extent( *rgn );
	
	region_scanner scanner( extent, temp_space, rowBytes );
	
	const UInt16* p = (const UInt16*) bitmap->baseAddr;
	
	const UInt16* prev = temp_space;
	
	do
	{
		scanner.scan( bounds.left, v, p, prev, margin );
		
		prev = p;
		
		p += rowBytes / 2u;
	}
	while ( ++v < bounds.bottom );
	
	scanner.finish( bounds.left, v, prev, margin );
	
	finish_region( rgn, max_rgn_size + rowBytes );
	
	return noErr;
}
