/*
	Drag.cc
	-------
*/

#include "Drag.hh"

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif

// mac-glue-utils
#include "mac_glue/OSUtils.hh"

// ams-common
#include "QDGlobals.hh"
#include "raster_lock.hh"

// ams-ui
#include "patches.hh"


#pragma exceptions off


#ifndef __CONTROLS__

enum
{
	hAxisOnly = 1,
	vAxisOnly = 2,
};

#endif

Point     Mouse       : 0x0830;
GrafPtr   WMgrPort    : 0x09DE;
RgnHandle GrayRgn     : 0x09EE;
Pattern   DragPattern : 0x0A34;

/*
	TODO:  DragGrayRgn() and DragTheRgn() don't yet support the following:
	
	 * action proc
*/

pascal long DragGrayRgn_patch( RgnHandle    rgn,
                               Point        start,
                               const Rect*  limit,
                               const Rect*  slop,
                               short        axis,
                               action_proc  action )
{
	QDGlobals& qd = get_QDGlobals();
	
	DragPattern = qd.gray;
	
	return DragTheRgn( rgn, start, limit, slop, axis, action );
}

static inline
long PinRect_pt( const Rect* rect, Point pt )
{
	if ( pt.h > rect->right )
	{
		pt.h = rect->right;
	}
	
	if ( pt.v > rect->bottom )
	{
		pt.v = rect->bottom;
	}
	
	return PinRect( rect, pt );
}

pascal long DragTheRgn_patch( RgnHandle    rgn,
                              Point        start,
                              const Rect*  limit,
                              const Rect*  slop,
                              short        axis,
                              action_proc  action )
{
	RgnHandle tmp = NewRgn();
	RgnHandle mouseRgn = tmp;
	
	/*
		Instead of calling FrameRgn(), which constructs a temporary frame
		region each time it's called, factor out the frame construction
		and call PaintRgn() to paint it directly.
	*/
	
	CopyRgn( rgn, tmp );
	
	InsetRgn( tmp, 1, 1 );
	
	XorRgn( rgn, tmp, rgn );
	
	QDGlobals& qd = get_QDGlobals();
	
	GrafPtr saved_port = qd.thePort;
	
	const short dh = -saved_port->portBits.bounds.left;
	const short dv = -saved_port->portBits.bounds.top;
	
	start.h += dh;
	start.v += dv;
	
	Rect limitRect;
	Rect slopRect;
	
	if ( dh != 0  ||  dv != 0 )
	{
		limitRect = *limit;
		slopRect  = *slop;
		
		OffsetRect( &limitRect, dh, dv );
		OffsetRect( &slopRect,  dh, dv );
		
		limit = &limitRect;
		slop  = &slopRect;
	}
	
	PenState saved_penState;
	GetPenState( &saved_penState );
	
	/*
		Use notPatXor instead of patXor, so when used on a background of
		qd.gray, the frame appears solid black instead of solid white.
	*/
	PenMode( notPatXor );
	
	PenPat( &DragPattern );
	
	Point pt = start;
	
	SetRectRgn( mouseRgn, pt.h, pt.v, pt.h + 1, pt.v + 1 );
	
	PaintRgn( rgn );
	
	bool was_inside = true;
	
	long sleep = 0;
	
	while ( true )
	{
		EventRecord event;
		
		if ( is_StillDown_patched() )
		{
			if ( ! StillDown() )
			{
				break;
			}
			
			mac::glue::delay( 1 );
			
			event.where = Mouse;
		}
		else
		{
			bool eventful = WaitNextEvent( mUpMask, &event, sleep, mouseRgn );
			
			// Events for DAs won't occur here, so we don't need to check.
			
			if ( event.what == mouseUp )
			{
				break;
			}
			
			const Point& where = event.where;
			
			SetRectRgn( mouseRgn, where.h, where.v, where.h + 1, where.v + 1 );
			
			if ( event.what != nullEvent )
			{
				/*
					Discard any mouse-moved events without redrawing the region.
					This can be prohibitively CPU-taxing on lightweight systems.
					
					Set sleep to 0 to ensure that we get a null event afterward.
				*/
				
				sleep = 0;
				
				continue;
			}
			
			/*
				We got a null event -- redraw the dotted region outline (if it
				actually moved, that is), and set sleep back to forever (approx).
			*/
			
			sleep = 0xFFFFFFFF;
		}
		
		/*
			Bounds-check the actual cursor location
			before applying any modifications to it.
		*/
		
		const bool is_inside = PtInRect( event.where, slop );
		
		switch ( axis )
		{
			case hAxisOnly:  event.where.v = pt.v;  break;
			case vAxisOnly:  event.where.h = pt.h;  break;
			
			default:
				break;
		}
		
		long where = PinRect_pt( limit, event.where );
		
		if ( is_inside | was_inside  &&  *(long*) &pt != where )
		{
			raster_lock lock;
			
			if ( was_inside )
			{
				PaintRgn( rgn );
			}
			
			if ( is_inside )
			{
				const short dh =  where        - pt.h;
				const short dv = (where >> 16) - pt.v;
				
				OffsetRgn( rgn, dh, dv );
				
				PaintRgn( rgn );
				
				*(long*) &pt = where;
			}
			
			was_inside = is_inside;
		}
	}
	
	if ( was_inside )
	{
		PaintRgn( rgn );
	}
	
	SetPenState( &saved_penState );
	
	DisposeRgn( tmp );
	
	if ( was_inside )
	{
		Point delta = { pt.v - start.v, pt.h - start.h };
		
		return *(long*) &delta;
	}
	
	return 0x80008000;
}
