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


static
long WDEF_0_Draw( short varCode, GrafPort* w, long param )
{
	WindowPeek window = (WindowPeek) w;
	
	FrameRect( &window->strucRgn[0]->rgnBBox );
	
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
