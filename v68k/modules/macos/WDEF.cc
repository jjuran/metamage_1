/*
	WDEF.cc
	-------
*/

#include "WDEF.hh"

// Mac OS
#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif


static inline
short shadow_for_variant( short varCode )
{
	const short shadow_px = varCode == altDBoxProc ? 2
	                      :                          0;
	
	return shadow_px;
}

static
long WDEF_0_Draw( short varCode, GrafPort* w, long param )
{
	WindowPeek window = (WindowPeek) w;
	
	const Rect& content = window->contRgn[0]->rgnBBox;
	
	Rect frame = content;
	
	InsetRect( &frame, -1, -1 );
	FrameRect( &frame );
	
	if ( const short shadow_px = shadow_for_variant( varCode ) )
	{
		OffsetRect( &frame, shadow_px, shadow_px );
		
		const Point topLeft = (const Point&) frame;
		
		frame.left = frame.right - shadow_px;
		PaintRect( &frame );
		frame.left = topLeft.h;
		
		frame.top = frame.bottom - shadow_px;
		PaintRect( &frame );
	}
	
	return 0;
}

static
long WDEF_0_Hit( short varCode, GrafPort* w, Point where )
{
	WindowPeek window = (WindowPeek) w;
	
	if ( PtInRect( where, &window->contRgn[0]->rgnBBox ) )
	{
		return wInContent;
	}
	
	return wNoHit;
}

static
long WDEF_0_CalcRgns( short varCode, WindowPtr w )
{
	WindowPeek window = (WindowPeek) w;
	
	const Rect& bounds = w->portBits.bounds;
	
	Rect rect = w->portRect;
	
	OffsetRect( &rect, -bounds.left, -bounds.top );
	
	RectRgn( window->contRgn, &rect );
	
	InsetRect( &rect, -1, -1 );
	
	RectRgn( window->strucRgn, &rect );
	
	if ( const short shadow_px = shadow_for_variant( varCode ) )
	{
		OffsetRect( &rect, shadow_px, shadow_px );
		
		RgnHandle shadow = NewRgn();
		
		RectRgn( shadow, &rect );
		
		UnionRgn( window->strucRgn, shadow, window->strucRgn );
		
		DisposeRgn( shadow );
	}
	
	return 0;
}

long WDEF_0( short varCode, WindowPtr w, short message, long param )
{
	switch ( message )
	{
		case wDraw:
			return WDEF_0_Draw( varCode, w, param );
		
		case wHit:
			return WDEF_0_Hit( varCode, w, *(Point*) &param );
		
		case wCalcRgns:
			return WDEF_0_CalcRgns( varCode, w );
		
		case wNew:
		case wDispose:
		case wGrow:
		case wDrawGIcon:
		default:
			break;
	}
	
	return 0;
}
