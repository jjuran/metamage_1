/*
	regions.cc
	----------
*/

#include "regions.hh"

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#endif


RgnHandle allocRgns[ 10 ];
RgnHandle mouseRgns[ 10 ];
RgnHandle otherRgn;

static RgnHandle tmp = NewRgn();

RgnHandle alloc_mouseRgns()
{
	for ( short i = 0;  i < 10;  ++i )
	{
		allocRgns[ i ] =
		mouseRgns[ i ] = NewRgn();
	}
	
	return otherRgn = mouseRgns[ 0 ];
}

void calibrate_mouseRgns( Point globalOffset, short unitLength )
{
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

RgnHandle deactivate_region( int rgn_index )
{
	RgnHandle rgn = mouseRgns[ rgn_index ];
	
	mouseRgns[ rgn_index ] = otherRgn;
	
	XorRgn( otherRgn, rgn, otherRgn );
	
	return otherRgn;
}

RgnHandle reactivate_region( int rgn_index )
{
	RgnHandle rgn = mouseRgns[ rgn_index ] = allocRgns[ rgn_index ];
	
	XorRgn( otherRgn, rgn, otherRgn );
	
	return rgn;
}

RgnHandle get_X_token( short unitLength )
{
	static short prevLen = 0;
	static RgnHandle rgn = NewRgn();
	
	if ( unitLength == prevLen )
	{
		return rgn;
	}
	
	const short width  = unitLength * 6;
	const short height = unitLength * 6;
	
	const short offset = unitLength > 1 ? unitLength * 3 / 4u : 1;
	
	PolyHandle dexter = OpenPoly();
	
	MoveTo( 0,              0      + offset );
	LineTo( 0     + offset, 0               );
	LineTo( width,          height - offset );
	LineTo( width - offset, height          );
	LineTo( 0,              0      + offset );
	
	ClosePoly();
	
	PolyHandle sinister = OpenPoly();
	
	MoveTo( width - offset, 0               );
	LineTo( width,          0      + offset );
	LineTo( 0     + offset, height          );
	LineTo( 0,              height - offset );
	LineTo( width - offset, 0               );
	
	ClosePoly();
	
	OpenRgn();
	FramePoly( dexter );
	CloseRgn( tmp );
	
	OpenRgn();
	FramePoly( sinister );
	CloseRgn( rgn );
	
	UnionRgn( tmp, rgn, rgn );
	
	prevLen = unitLength;
	
	KillPoly( dexter   );
	KillPoly( sinister );
	
	return rgn;
}

RgnHandle get_O_token( short unitLength )
{
	static short prevLen = 0;
	static RgnHandle rgn = NewRgn();
	
	if ( unitLength == prevLen )
	{
		return rgn;
	}
	
	const short width  = unitLength * 6;
	const short height = unitLength * 6;
	
	Rect bounds = { 0, 0, height, width };
	
	OpenRgn();
	FrameOval( &bounds );
	CloseRgn( rgn );
	
	InsetRect( &bounds, unitLength, unitLength );
	
	OpenRgn();
	FrameOval( &bounds );
	CloseRgn( tmp );
	
	XorRgn( rgn, tmp, rgn );
	
	return rgn;
}
