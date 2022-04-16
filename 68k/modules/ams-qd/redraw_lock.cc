/*
	redraw_lock.cc
	--------------
*/

#include "redraw_lock.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


char* ScrnBase : 0x0824;
Rect  CrsrRect : 0x083C;


static inline
bool intersects( const Rect& a, const Rect& b, short dh, short dv )
{
	return
		a.top < b.bottom - dv  &&
		b.top < a.bottom + dv  &&
		a.left < b.right - dh  &&
		b.left < a.right + dh;
}

static inline
bool onscreen( const BitMap& bitmap )
{
	return bitmap.baseAddr == ScrnBase;
}

static inline
bool intersects_cursor( const BitMap& bitmap, const Rect& r )
{
	const short dv = bitmap.bounds.top;
	const short dh = bitmap.bounds.left;
	
	return onscreen( bitmap )  &&  intersects( CrsrRect, r, dh, dv );
}

redraw_lock::redraw_lock( const BitMap& bitmap, const Rect& workArea )
:
	its_raster_lock( intersects_cursor( bitmap, workArea ), onscreen( bitmap ) )
{
}

redraw_lock::redraw_lock( const BitMap& bits1, const Rect& rect1,
                          const BitMap& bits2, const Rect& rect2 )
:
	its_raster_lock( intersects_cursor( bits1, rect1 )  ||
	                 intersects_cursor( bits2, rect2 ),
	                 onscreen( bits1 )  ||
	                 intersects_cursor( bits2, rect2 ) )
{
}
