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


/*
	This is the height added by a title bar.  This means that in window
	coordinates, the top of the structure region is -19 instead of -1.
*/
const short title_bar_height = 18;

const short stripes_v_offset = 3;
const short stripes_h_offset = 1;

const short close_box_v_offset = 3;
const short close_box_h_offset = 8;
const short close_box_margin   = 1;
const short close_box_width    = 11;

static inline
short shadow_for_variant( short varCode )
{
	const short shadow_px = varCode == altDBoxProc ? 2
	                      : (varCode & 3) == 0     ? 1
	                      :                          0;
	
	return shadow_px;
}

static
long WDEF_0_Draw( short varCode, GrafPort* w, long param )
{
	WindowPeek window = (WindowPeek) w;
	
	const Rect& content = window->contRgn[0]->rgnBBox;
	
	Rect frame = content;
	
	const bool has_title_bar = (varCode & 3) == 0;
	
	if ( has_title_bar )
	{
		frame.top -= title_bar_height;
	}
	
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
	
	if ( ! has_title_bar )
	{
		return 0;
	}
	
	Rect edge = content;
	edge.bottom = edge.top--;
	
	Rect title_bar =
	{
		content.top - title_bar_height,
		content.left,
		edge.top,
		content.right,
	};
	
	EraseRect( &title_bar );
	
	PaintRect( &edge );
	
	if ( ! window->hilited )
	{
		return 0;
	}
	
	InsetRect( &title_bar, stripes_h_offset, stripes_v_offset );
	
	Pattern odd_stripes  = { 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00 };
	Pattern even_stripes = { 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF };
	
	FillRect( &title_bar, content.top & 1 ? &odd_stripes : &even_stripes );
	
	if ( window->goAwayFlag )
	{
		Rect box = title_bar;
		
		box.left += close_box_h_offset - close_box_margin - stripes_h_offset;
		box.right = box.left + close_box_width + close_box_margin * 2;
		
		EraseRect( &box );
		InsetRect( &box, close_box_margin, 0 );
		FrameRect( &box );
	}
	
	return 0;
}

static
long WDEF_0_Hit( short varCode, GrafPort* w, Point where )
{
	WindowPeek window = (WindowPeek) w;
	
	const Rect& content = window->contRgn[0]->rgnBBox;
	
	if ( PtInRect( where, &content ) )
	{
		return wInContent;
	}
	
	const bool has_title_bar = (varCode & 3) == 0;
	
	if ( has_title_bar  &&  where.v < content.top )
	{
		const Rect& frame = window->strucRgn[0]->rgnBBox;
		
		const bool in_title_bar = where.v >= frame.top   &&
		                          where.h >= frame.left  &&
		                          where.h <= content.right;
		
		if ( in_title_bar )
		{
			const short top  = frame.top  + 1 + close_box_v_offset;
			const short left = frame.left + 1 + close_box_h_offset;
			
			const Rect close_box =
			{
				top,
				left,
				top  + close_box_width,
				left + close_box_width,
			};
			
			if ( window->goAwayFlag  &&  PtInRect( where, &close_box ) )
			{
				return wInGoAway;
			}
			
			return wInDrag;
		}
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
	
	const bool has_title_bar = (varCode & 3) == 0;
	
	if ( has_title_bar )
	{
		rect.top -= title_bar_height;
	}
	
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
