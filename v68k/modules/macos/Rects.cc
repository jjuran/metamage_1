/*
	Rects.cc
	--------
*/

#include "Rects.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// Standard C
#include <string.h>

// macos
#include "QDGlobals.hh"


class white_t {};
class black_t {};

static const white_t White = white_t();
static const black_t Black = black_t();

static inline bool operator==( const Pattern& a, white_t )
{
	const UInt32* p = (const UInt32*) a.pat;
	
	return *p++ == '\0'  &&  *p == '\0';
}

static inline bool operator==( const Pattern& a, black_t )
{
	const UInt32* p = (const UInt32*) a.pat;
	
	return *p++ == 0xFFFFFFFF  &&  *p == 0xFFFFFFFF;
}

struct rectangular_op_params
{
	Point topLeft;
	
	Ptr start;
	
	uint16_t height;
	
	uint16_t draw_bytes;
	uint16_t skip_bytes;
	
	uint8_t left_mask;
	uint8_t right_mask;
	
	Pattern*  pattern;
	short     origin_h;
};

static inline short min( short a, short b )
{
	return b < a ? b : a;
}

static inline short max( short a, short b )
{
	return a > b ? a : b;
}

static void do_Rect_intersection( Rect& result, const Rect& a, const Rect& b )
{
	result.top    = max( a.top,    b.top    );
	result.left   = max( a.left,   b.left   );
	result.bottom = min( a.bottom, b.bottom );
	result.right  = min( a.right,  b.right  );
}

static void get_rectangular_op_params_for_rect( rectangular_op_params&  params,
                                                const Rect&             input_rect )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	const BitMap& portBits = thePort->portBits;
	
	const Rect& bounds = portBits.bounds;
	
	Rect rect;
	
	do_Rect_intersection( rect, bounds, input_rect );
	
	const int width_px  = rect.right - rect.left;
	const int height_px = rect.bottom - rect.top;
	
	if ( width_px <= 0  ||  height_px <= 0 )
	{
		params.height = 0;
		
		return;
	}
	
	const unsigned top  = rect.top  - bounds.top;
	const unsigned left = rect.left - bounds.left;
	
	const unsigned right = left + width_px;
	
	const unsigned outer_left_bytes  =  left      / 8;
	const unsigned inner_left_bytes  = (left + 7) / 8;
	const unsigned inner_right_bytes =  right      / 8;
	const unsigned outer_right_bytes = (right + 7) / 8;
	
	const unsigned inner_bytes = inner_right_bytes - inner_left_bytes;
	const unsigned outer_bytes = outer_right_bytes - outer_left_bytes;
	
	const uint8_t left_mask  = (left  & 0x7) ? ~((1 << 8 - (left  & 0x7)) - 1) : 0;
	const uint8_t right_mask = (right & 0x7) ?   (1 << 8 - (right & 0x7)) - 1  : 0;
	
	const uint32_t rowBytes = portBits.rowBytes;
	
	params.topLeft    = *(Point*) &rect;
	params.start      = portBits.baseAddr + top * rowBytes + outer_left_bytes;
	params.height     = height_px;
	params.draw_bytes = inner_bytes;
	params.skip_bytes = rowBytes - outer_bytes;
	params.left_mask  = left_mask;
	params.right_mask = right_mask;
	
	if ( int( inner_bytes ) < 0 )
	{
		params.left_mask  |= right_mask;
		params.right_mask  = 0;
		
		params.draw_bytes = 0;
	}
}

static void erase_rect( const rectangular_op_params& params )
{
	Ptr p = params.start;
	
	for ( int i = params.height;  i > 0;  --i )
	{
		if ( params.left_mask )
		{
			*p++ &= params.left_mask;
		}
		
		memset( p, '\0', params.draw_bytes );
		
		p += params.draw_bytes;
		
		if ( params.right_mask )
		{
			*p++ &= params.right_mask;
		}
		
		p += params.skip_bytes;
	}
}

static void paint_rect( const rectangular_op_params& params )
{
	Ptr p = params.start;
	
	for ( int i = params.height;  i > 0;  --i )
	{
		if ( params.left_mask )
		{
			*p++ |= ~params.left_mask;
		}
		
		memset( p, 0xFF, params.draw_bytes );
		
		p += params.draw_bytes;
		
		if ( params.right_mask )
		{
			*p++ |= ~params.right_mask;
		}
		
		p += params.skip_bytes;
	}
}

static void invert_rect( const rectangular_op_params& params )
{
	Ptr p = params.start;
	
	for ( int i = params.height;  i > 0;  --i )
	{
		if ( params.left_mask )
		{
			char out = *p &  params.left_mask;
			char in = ~*p & ~params.left_mask;
			
			*p++ = out | in;
		}
		
		for ( int n = params.draw_bytes;  n > 0;  --n )
		{
			*p = ~*p;
			
			++p;
		}
		
		if ( params.right_mask )
		{
			char in = ~*p & ~params.right_mask;
			char out = *p &  params.right_mask;
			
			*p++ = in | out;
		}
		
		p += params.skip_bytes;
	}
}

static void frame_rect( const Rect* rect )
{
	if ( rect->top >= rect->bottom  ||  rect->left >= rect->right )
	{
		return;
	}
	
	if ( rect->bottom - rect->top <= 2  ||  rect->right - rect->left <= 2 )
	{
		StdRect_patch( kQDGrafVerbPaint, rect );
		
		return;
	}
	
	Rect edge;
	
	*(Point*) &edge = *(Point*) rect;  // topLeft
	
	edge.right = rect->right;
	
	edge.bottom = edge.top + 1;
	
	StdRect_patch( kQDGrafVerbPaint, &edge );
	
	
	++edge.top;
	
	edge.right = edge.left + 1;
	
	edge.bottom = rect->bottom - 1;
	
	StdRect_patch( kQDGrafVerbPaint, &edge );
	
	
	edge.left  = rect->right - 1;
	edge.right = rect->right;
	
	StdRect_patch( kQDGrafVerbPaint, &edge );
	
	edge.left = rect->left;
	
	edge.top = edge.bottom;
	
	++edge.bottom;
	
	StdRect_patch( kQDGrafVerbPaint, &edge );
}

static void fill_rect( const rectangular_op_params& params )
{
	Pattern& pattern = *params.pattern;
	
	short       v = params.topLeft.v & 0x7;
	short const h = params.origin_h & 0x07;
	
	if ( h != 0 )
	{
		for ( int i = 0;  i < 8;  ++i )
		{
			pattern.pat[i] = pattern.pat[i] <<      h
			               | pattern.pat[i] >> (8 - h);
		}
	}
	
	Ptr p = params.start;
	
	for ( int i = params.height;  i > 0;  --i )
	{
		const uint8_t pat = pattern.pat[v];
		
		if ( params.left_mask )
		{
			*p = (*p  &  params.left_mask)
			   | (pat & ~params.left_mask);
			
			++p;
		}
		
		memset( p, pat, params.draw_bytes );
		
		p += params.draw_bytes;
		
		if ( params.right_mask )
		{
			*p = (*p  &  params.right_mask)
			   | (pat & ~params.right_mask);
			
			++p;
		}
		
		p += params.skip_bytes;
		
		v = (v + 1) & 0x7;
	}
}

pascal void StdRect_patch( signed char verb, const Rect* r )
{
	if ( verb == kQDGrafVerbFrame )
	{
		frame_rect( r );
		
		return;
	}
	
	GrafPort& port = **get_addrof_thePort();
	
	Rect clipRect = port.clipRgn[0]->rgnBBox;
	
	SectRect( r, &clipRect, &clipRect );
	
	rectangular_op_params params;
	
	get_rectangular_op_params_for_rect( params, clipRect );
	
	params.pattern = &port.fillPat;
	params.origin_h = port.portBits.bounds.left;
	
	if ( verb == kQDGrafVerbErase )
	{
		if ( port.bkPat == White )
		{
			erase_rect( params );
			
			return;
		}
		
		port.fillPat = port.bkPat;
		
		verb = kQDGrafVerbFill;
	}
	else if ( verb == kQDGrafVerbPaint )
	{
		if ( port.pnPat == Black )
		{
			paint_rect( params );
			
			return;
		}
		
		port.fillPat = port.pnPat;
		
		verb = kQDGrafVerbFill;
	}
	else if ( verb == kQDGrafVerbInvert )
	{
		invert_rect( params );
		
		return;
	}
	
	switch ( verb )
	{
		case kQDGrafVerbFill:    fill_rect  ( params );  break;
		
		default:
			break;
	}
}

pascal void EraseRect_patch( const Rect* rect )
{
	StdRect( kQDGrafVerbErase, rect );
}

pascal void PaintRect_patch( const Rect* rect )
{
	StdRect( kQDGrafVerbPaint, rect );
}

pascal void InverRect_patch( const Rect* rect )
{
	StdRect( kQDGrafVerbInvert, rect );
}

pascal void FrameRect_patch( const Rect* rect )
{
	StdRect( kQDGrafVerbFrame, rect );
}

pascal void FillRect_patch( const Rect* rect, const Pattern* pattern )
{
	GrafPort& port = **get_addrof_thePort();
	
	port.fillPat = *pattern;
	
	StdRect( kQDGrafVerbFill, rect );
}

pascal unsigned char EqualRect_patch( const Rect* a, const Rect* b )
{
	return a->top    == b->top     &&
	       a->left   == b->left    &&
	       a->bottom == b->bottom  &&
	       a->right  == b->right;
}

pascal void SetRect_patch( Rect*  rect,
                           short  left,
                           short  top,
                           short  right,
                           short  bottom )
{
	rect->top    = top;
	rect->left   = left;
	rect->bottom = bottom;
	rect->right  = right;
}

pascal void OffsetRect_patch( Rect* rect, short dh, short dv )
{
	rect->top    += dv;
	rect->left   += dh;
	rect->bottom += dv;
	rect->right  += dh;
}

pascal void InsetRect_patch( Rect* rect, short dh, short dv )
{
	rect->top    += dv;
	rect->left   += dh;
	rect->bottom -= dv;
	rect->right  -= dh;
}

pascal unsigned char SectRect_patch( const Rect* a, const Rect* b, Rect* c )
{
	c->top    = a->top    > b->top    ? a->top    : b->top;
	c->left   = a->left   > b->left   ? a->left   : b->left;
	c->bottom = b->bottom < a->bottom ? b->bottom : a->bottom;
	c->right  = b->right  < a->right  ? b->right  : a->right;
	
	if ( EmptyRect_patch( c ) )
	{
		c->top    = 0;
		c->left   = 0;
		c->bottom = 0;
		c->right  = 0;
		
		return false;
	}
	
	return true;
}

pascal void UnionRect_patch( const Rect* a, const Rect* b, Rect* c )
{
	c->top    = b->top    < a->top    ? b->top    : a->top;
	c->left   = b->left   < a->left   ? b->left   : a->left;
	c->bottom = a->bottom > b->bottom ? a->bottom : b->bottom;
	c->right  = a->right  > b->right  ? a->right  : b->right;
}

pascal void Pt2Rect_patch( Point a, Point b, Rect* c )
{
	if ( b.v < a.v )
	{
		c->top    = b.v;
		c->bottom = a.v;
	}
	else
	{
		c->top    = a.v;
		c->bottom = b.v;
	}
	
	if ( b.h < a.h )
	{
		c->left  = b.h;
		c->right = a.h;
	}
	else
	{
		c->left  = a.h;
		c->right = b.h;
	}
}

pascal unsigned char PtInRect_patch( Point pt, const Rect* rect )
{
	return rect->top  <= pt.v  &&  pt.v < rect->bottom  &&  
	       rect->left <= pt.h  &&  pt.h < rect->right;
}

pascal unsigned char EmptyRect_patch( const Rect* rect )
{
	return rect->top >= rect->bottom  ||  rect->left >= rect->right;
}
