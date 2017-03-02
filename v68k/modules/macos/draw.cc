/*
	draw.cc
	-------
*/

#include "draw.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


void get_refined_clip_region( const GrafPort&  port,
                              const Rect&      dstRect,
                              RgnHandle        result )
{
	/*
		csdx, csdy:  Coordinate system deltas
		
		These are deltas from the global coordinate system to the local
		coordinate system.  Add them to global points to get local, and
		subtract them from local points to get global.
		
		Note that for a local origin below and to the right of the global
		origin, these values will be negative -- for example, to get from
		local (0,0) to global (100,80), you must add positive numbers or
		subtract negative ones.
	*/
	
	const short csdx = port.portBits.bounds.left;
	const short csdy = port.portBits.bounds.top;
	
	Rect clipRect = port.clipRgn[0]->rgnBBox;
	
	SectRect( &dstRect,              &clipRect, &clipRect );
	SectRect( &port.portBits.bounds, &clipRect, &clipRect );
	
	OffsetRect( &clipRect, -csdx, -csdy );  // convert to global coordinates
	
	RectRgn( result, &clipRect );
	
	SectRgn( port.visRgn, result, result );
	
	OffsetRgn( result, csdx, csdy );  // convert back to local coordinates
	
	SectRgn( port.clipRgn, result, result );
}
