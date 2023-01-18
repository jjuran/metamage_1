/*
	WDEF-1.cc
	---------
*/

// Mac OS
#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif

// iota
#include "iota/freestore_free.hh"

// ams-common
#include "QDGlobals.hh"
#include "raster_lock.hh"


#pragma exceptions off


static
pascal long WDEF_1( short varCode, WindowPtr w, short message, long param );

asm int main()
{
	JMP      WDEF_1
}

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

const short close_box_v_offset = 3;
const short close_box_h_offset = 8;
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

#define PACK7( _6, _5, _4, _3, _2, _1, _0 )  \
	PACK16( _6, _5, _4, _3, _2, _1, _0, 0, 0, 0, 0, 0, 0, 0, 0, 0 )

#define _ 0
#define X 1

static const uint16_t splat_bits[] =
{
	PACK7( _,_,_,X,_,_,_ ),
	PACK7( X,_,_,X,_,_,X ),
	PACK7( _,X,_,X,_,X,_ ),
	PACK7( _,_,_,_,_,_,_ ),
	PACK7( X,X,X,_,X,X,X ),
	PACK7( _,_,_,_,_,_,_ ),
	PACK7( _,X,_,X,_,X,_ ),
	PACK7( X,_,_,X,_,_,X ),
	PACK7( _,_,_,X,_,_,_ ),
};

static BitMap splat_bitmap =
{
	(Ptr) splat_bits,
	2,
	{ 0, 0, 9, 7 },
};

static
RgnHandle make_splat()
{
	RgnHandle rgn = NewRgn();
	
	BitMapToRegion( rgn, &splat_bitmap );
	
	return rgn;
}

static
long toggle_close_box( WindowPeek window )
{
	static RgnHandle splat_rgn = make_splat();
	
	const Rect& frame = window->strucRgn[0]->rgnBBox;
	
	/*
		Add one to the frame coordinate to get inside the frame.
		Add one (or two) to the box coordinate to get inside the box.
		
		(Unlike the WDEF 0 close box, WDEF 1's splat has a bounding rectangle
		that isn't square -- it's narrower in width than it is tall.)
	*/
	
	const short frame_thickness = 1;
	
	const short top  = frame.top  + frame_thickness + close_box_v_offset + 1;
	const short left = frame.left + frame_thickness + close_box_h_offset + 2;
	
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
pascal long WDEF_1_Draw( short varCode, GrafPort* w, long param )
{
	WindowPeek window = (WindowPeek) w;
	
	if ( param == wInGoAway )
	{
		return toggle_close_box( window );
	}
	
	GrafPtr   thePort = *get_addrof_thePort();
	RgnHandle clipRgn = thePort->clipRgn;
	
	raster_lock lock;
	
	static RgnHandle tmp = NewRgn();
	
	CopyRgn( window->strucRgn, tmp );
	InsetRgn( tmp, 1, 1 );
	
	/*
		TODO:  We calculate the inset region twice -- once above, and
		again below in FrameRgn().  Maybe this could be deduplicated?
	*/
	
	FrameRgn( window->strucRgn );
	
	SectRgn( clipRgn, tmp, clipRgn );
	
	const Rect& content = window->contRgn[0]->rgnBBox;
	
	Rect frame = content;
	
	const bool has_title_bar = true;
	
	if ( has_title_bar )
	{
		frame.top -= title_bar_height;
	}
	
	const short frame_inset = -1;
	
	InsetRect( &frame, frame_inset, frame_inset );
	
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
	
	StringPtr title = window->titleHandle ? *window->titleHandle : NULL;
	
	if ( window->titleWidth < 0 )
	{
		window->titleWidth = title ? StringWidth( title ) : 0;
	}
	
	if ( window->titleWidth != 0 )
	{
		short left = content.left;
		
		if ( window->goAwayFlag )
		{
			left += close_box_h_offset + close_box_width;
		}
		
		const short h = (left + content.right - window->titleWidth) / 2;
		
		MoveTo( h, title_bar.top + title_baseline_v );
		
		DrawString( title );
	}
	
	if ( ! window->hilited )
	{
		return 0;
	}
	
	if ( window->goAwayFlag )
	{
		Rect box = title_bar;
		
		InsetRect( &box, 0, stripes_v_offset );
		
		box.left += close_box_h_offset;
		box.right = box.left + close_box_width;
		
		FrameRect( &box );
	}
	
	InvertRect( &title_bar );
	
	return 0;
}

static
long WDEF_1_Hit( short varCode, GrafPort* w, Point where )
{
	WindowPeek window = (WindowPeek) w;
	
	if ( PtInRgn( where, window->contRgn ) )
	{
		return wInContent;
	}
	
	const Rect& content = window->contRgn[0]->rgnBBox;
	
	if ( where.v < content.top )
	{
		const bool in_title_bar = PtInRgn( where, window->strucRgn );
		
		if ( in_title_bar )
		{
			const Rect& frame = window->strucRgn[0]->rgnBBox;
			
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
long WDEF_1_CalcRgns( short varCode, WindowPtr w )
{
	WindowPeek window = (WindowPeek) w;
	
	const Rect& bounds = w->portBits.bounds;
	
	Rect rect = w->portRect;
	
	OffsetRect( &rect, -bounds.left, -bounds.top );
	
	RectRgn( window->contRgn, &rect );
	
	const short frame_inset = -1;
	
	InsetRect( &rect, frame_inset, frame_inset );
	
	rect.top -= title_bar_height;
	
	OpenRgn();
	FrameRoundRect( &rect, 16, 16 );
	CloseRgn( window->strucRgn );
	
	RgnHandle tmp = NewRgn();
	
	CopyRgn( window->strucRgn, tmp );
	
	InsetRgn( tmp, 1, 1 );
	
	SectRgn( window->contRgn, tmp, window->contRgn );
	
	DisposeRgn( tmp );
	
	return 0;
}

pascal long WDEF_1( short varCode, WindowPtr w, short message, long param )
{
	switch ( message )
	{
		case wDraw:
			return WDEF_1_Draw( varCode, w, param );
		
		case wHit:
			return WDEF_1_Hit( varCode, w, *(Point*) &param );
		
		case wCalcRgns:
			return WDEF_1_CalcRgns( varCode, w );
		
		case wNew:
		case wDispose:
			break;
		
		case wGrow:
		case wDrawGIcon:
		default:
			break;
	}
	
	return 0;
}
