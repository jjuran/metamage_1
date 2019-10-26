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

// ams-common
#include "QDGlobals.hh"
#include "raster_lock.hh"


/*
	This is the height added by a title bar.  This means that in window
	coordinates, the top of the structure region is -19 instead of -1.
*/
const short title_bar_height = 18;

const short title_bar_interior_height = title_bar_height - 1;

const short system_font_height = 15;
const short system_font_ascent = 12;

const short title_offset = (title_bar_interior_height - system_font_height) / 2;

const short title_baseline_v = title_offset + system_font_ascent;

const short stripes_v_offset = 3;
const short stripes_h_offset = 1;

const short close_box_v_offset = 3;
const short close_box_h_offset = 8;
const short close_box_margin   = 1;
const short close_box_width    = 11;

#define PACK16( _15, _14, _13,_12,  \
                _11, _10,  _9, _8,  \
                 _7,  _6,  _5, _4,  \
                 _3,  _2,  _1, _0 ) \
	(                            \
		+ (_15 << 15)  \
		| (_14 << 14)  \
		| (_13 << 13)  \
		| (_12 << 12)  \
		| (_11 << 11)  \
		| (_10 << 10)  \
		| ( _9 <<  9)  \
		| ( _8 <<  8)  \
		| ( _7 <<  7)  \
		| ( _6 <<  6)  \
		| ( _5 <<  5)  \
		| ( _4 <<  4)  \
		| ( _3 <<  3)  \
		| ( _2 <<  2)  \
		| ( _1 <<  1)  \
		| ( _0 <<  0)  \
	)

#define PACK9( _8, _7, _6, _5, _4, _3, _2, _1, _0 )  \
	PACK16( _8, _7, _6, _5, _4, _3, _2, _1, _0, 0, 0, 0, 0, 0, 0, 0 )

#define _ 0
#define X 1

static const uint16_t splat_bits[] =
{
	PACK9( _,_,_,_,X,_,_,_,_ ),
	PACK9( _,X,_,_,X,_,_,X,_ ),
	PACK9( _,_,X,_,X,_,X,_,_ ),
	PACK9( _,_,_,_,_,_,_,_,_ ),
	PACK9( X,X,X,_,_,_,X,X,X ),
	PACK9( _,_,_,_,_,_,_,_,_ ),
	PACK9( _,_,X,_,X,_,X,_,_ ),
	PACK9( _,X,_,_,X,_,_,X,_ ),
	PACK9( _,_,_,_,X,_,_,_,_ ),
};

static BitMap splat_bitmap =
{
	(Ptr) splat_bits,
	2,
	{ 0, 0, 9, 9 },
};

static
RgnHandle make_splat()
{
	RgnHandle rgn = NewRgn();
	
	BitMapToRegion( rgn, &splat_bitmap );
	
	return rgn;
}

static inline
short shadow_for_variant( short varCode )
{
	const short shadow_px = varCode == altDBoxProc ? 2
	                      : (varCode & 3) == 0     ? 1
	                      :                          0;
	
	return shadow_px;
}

static
RgnHandle rectangular_utility_region()
{
	static RgnHandle rgn = NewRgn();
	
	return rgn;
}

static
long toggle_close_box( WindowPeek window )
{
	static RgnHandle splat_rgn = make_splat();
	
	const Rect& frame = window->strucRgn[0]->rgnBBox;
	
	/*
		Add one to the frame coordinate to get inside the frame.
		Add one to the box coordinate to get inside the box.
	*/
	const short frame_thickness = 1;
	
	const short top  = frame.top  + frame_thickness + close_box_v_offset + 1;
	const short left = frame.left + frame_thickness + close_box_h_offset + 1;
	
	const short dv = top  - splat_rgn[0]->rgnBBox.top;
	const short dh = left - splat_rgn[0]->rgnBBox.left;
	
	if ( dh  ||  dv )
	{
		OffsetRgn( splat_rgn, dh, dv );
	}
	
	PenMode( patXor );
	
	PaintRgn( splat_rgn );
	
	PenNormal();
	
	return 0;
}

static
long WDEF_0_Draw( short varCode, GrafPort* w, long param )
{
	WindowPeek window = (WindowPeek) w;
	
	if ( param == wInGoAway )
	{
		return toggle_close_box( window );
	}
	
	raster_lock lock;
	
	const Rect& content = window->contRgn[0]->rgnBBox;
	
	Rect frame = content;
	
	const bool has_title_bar = (varCode & 3) == 0;
	
	if ( has_title_bar )
	{
		frame.top -= title_bar_height;
	}
	
	const short frame_inset = varCode == dBoxProc ? -8 : -1;
	
	InsetRect( &frame, frame_inset, frame_inset );
	FrameRect( &frame );
	
	if ( varCode == dBoxProc )
	{
		QDGlobals& qd = get_QDGlobals();
		
		PenSize( 2, 2 );
		
		InsetRect( &frame, 3, 3 );
		FrameRect( &frame );
		
		PenPat( &qd.white );
		
		InsetRect( &frame, -2, -2 );
		FrameRect( &frame );
		
		PenSize( 3, 3 );
		
		InsetRect( &frame, 4, 4 );
		FrameRect( &frame );
		
		PenNormal();
	}
	
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
	
	if ( const short title_width = window->titleWidth )
	{
		const short h = (content.left + content.right - title_width) / 2u;
		
		MoveTo( h, title_bar.top + title_baseline_v );
		
		DrawString( *window->titleHandle );
		
		if ( window->hilited )
		{
			QDGlobals& qd = get_QDGlobals();
			
			Rect title_area =
			{
				title_bar.top + stripes_v_offset,
				h - 6,
				title_bar.bottom - stripes_v_offset,
				h + title_width + 6,
			};
			
			RgnHandle title_region = rectangular_utility_region();
			
			RectRgn( title_region, &title_area );
			
			DiffRgn( qd.thePort->clipRgn, title_region, qd.thePort->clipRgn );
		}
	}
	
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
	
	if ( const bool has_grow_box = (varCode & 7) == 0 )
	{
		const Rect grow_box =
		{
			content.bottom - 15,
			content.right  - 15,
			content.bottom,
			content.right,
		};
		
		if ( PtInRect( where, &grow_box ) )
		{
			return wInGrow;
		}
	}
	
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
	
	const short frame_inset = varCode == dBoxProc ? -8 : -1;
	
	InsetRect( &rect, frame_inset, frame_inset );
	
	const bool has_title_bar = (varCode & 3) == 0;
	
	if ( has_title_bar )
	{
		rect.top -= title_bar_height;
	}
	
	RectRgn( window->strucRgn, &rect );
	
	if ( const short shadow_px = shadow_for_variant( varCode ) )
	{
		OffsetRect( &rect, shadow_px, shadow_px );
		
		RgnHandle shadow = rectangular_utility_region();
		
		RectRgn( shadow, &rect );
		
		UnionRgn( window->strucRgn, shadow, window->strucRgn );
	}
	
	return 0;
}

static
long WDEF_0_Grow( short varCode, WindowPtr w, const Rect& rect )
{
	WindowPeek window = (WindowPeek) w;
	
	if ( rect.top >= rect.bottom  ||  rect.left >= rect.right )
	{
		return 0;
	}
	
	Rect r =
	{
		rect.top   - 19,
		rect.left  -  1,
		rect.bottom + 1,
		rect.right  + 1,
	};
	
	if ( r.bottom < rect.top )
	{
		r.bottom = rect.top;
	}
	
	FrameRect( &r );
	
	++r.left;
	--r.right;
	
	r.top    = rect.top - 1;
	r.bottom = rect.top;
	
	FrameRect( &r );
	
	r.top    = rect.bottom - 15;
	r.bottom = rect.bottom - 14;
	
	if ( r.top > rect.top )
	{
		FrameRect( &r );
	}
	
	r.top    = rect.top;
	r.bottom = rect.bottom;
	
	r.left  = rect.right - 15;
	r.right = rect.right - 14;
	
	if ( r.left > rect.left )
	{
		FrameRect( &r );
	}
	
	return 0;
}

static
long WDEF_0_DrawGIcon( short varCode, WindowPtr w )
{
	const bool has_grow_box = (varCode & 0x7) == 0;
	
	if ( ! has_grow_box )
	{
		return 0;
	}
	
	WindowPeek window = (WindowPeek) w;
	
	const Rect& content = w->portRect;
	
	const Rect vline =
	{
		0,
		content.right - 15,
		content.bottom,
		content.right - 14,
	};
	
	const Rect hline =
	{
		content.bottom - 15,
		0,
		content.bottom - 14,
		content.right,
	};
	
	PaintRect( &vline );
	PaintRect( &hline );
	
	const Rect grow_box =
	{
		content.bottom - 14,
		content.right  - 14,
		content.bottom,
		content.right,
	};
	
	EraseRect( &grow_box );
	
	if ( ! window->hilited )
	{
		return 0;
	}
	
	const Rect mini_box =
	{
		grow_box.top  + 4,
		grow_box.left + 4,
		grow_box.bottom - 1,
		grow_box.right  - 1,
	};
	
	FrameRect( &mini_box );
	
	const Rect tiny_box =
	{
		grow_box.top  + 2,
		grow_box.left + 2,
		grow_box.bottom - 5,
		grow_box.right  - 5,
	};
	
	EraseRect( &tiny_box );
	FrameRect( &tiny_box );
	
	return 0;
}

pascal long WDEF_0( short varCode, WindowPtr w, short message, long param )
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
			break;
		
		case wGrow:
			return WDEF_0_Grow( varCode, w, *(Rect*) param );
		
		case wDrawGIcon:
			return WDEF_0_DrawGIcon( varCode, w );
		
		default:
			break;
	}
	
	return 0;
}
