/*
	Rects.cc
	--------
*/

#include "Rects.hh"

// Standard C
#include <stdlib.h>
#include <string.h>

// quickdraw
#include "qd/region_raster.hh"

// macos
#include "QDGlobals.hh"
#include "options.hh"


static inline asm void flush_screen()
{
	JSR  0xFFFFFFEE
}

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
	
	~rectangular_op_params()
	{
		if ( synchronized_quickdraw )
		{
			flush_screen();
		}
	}
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
                                                const Rect&             input_rect,
                                                bool                    clipping )
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
	
	char* baseAddr = portBits.baseAddr;
	
	if ( synchronized_quickdraw  &&  baseAddr == (char*) 0x0001A700 )
	{
		baseAddr += 0x00010000;
	}
	
	params.topLeft    = *(Point*) &rect;
	params.start      = baseAddr + top * rowBytes + outer_left_bytes;
	params.height     = height_px;
	params.skip_bytes = rowBytes - outer_bytes;
	
	if ( clipping )
	{
		params.draw_bytes = outer_bytes;
		params.left_mask  = 0;
		params.right_mask = 0;
	}
	else
	{
		params.draw_bytes = inner_bytes;
		params.left_mask  = left_mask;
		params.right_mask = right_mask;
	
		if ( int( inner_bytes ) < 0 )
		{
			params.left_mask  |= right_mask;
			params.right_mask  = 0;
		
			params.draw_bytes = 0;
		}
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

static void draw_rect( const rectangular_op_params&  params,
                       short                         pattern_transfer_mode,
                       const Rect&                   clipRect,
                       RgnHandle                     clipRgn )
{
	const short* bbox   = (short*) &clipRect;
	const short* extent = (short*) (clipRgn ? *clipRgn + 1 : NULL);
	
	unsigned size = quickdraw::region_raster::mask_size( bbox );
	
	char* temp = (char*) malloc( size );
	
	quickdraw::region_raster clip( bbox, extent, temp, size );
	
	Pattern& pattern = *params.pattern;
	
	if ( const bool negated = pattern_transfer_mode & 0x04 )
	{
		for ( int i = 0;  i < 8;  ++i )
		{
			pattern.pat[i] = ~pattern.pat[i];
		}
	}
	
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
	
	const short top    = params.topLeft.v;
	const short bottom = params.topLeft.v + params.height;
	
	for ( int i = top;  i < bottom;  ++i )
	{
		clip.load_mask( i );
		
		const char* clip_mask = temp;
		
		const uint8_t pat = pattern.pat[v];
		
		for ( uint16_t j = params.draw_bytes;  j > 0;  --j )
		{
			const uint8_t mask = *clip_mask++;
			
			const uint8_t src = pat & mask;
			
			switch ( pattern_transfer_mode & 0x03 )
			{
				// Use src vs. pat modes because we stripped off the 8 bit
				case srcCopy:  *p = (*p  & ~mask) |  src;  break;
				case srcOr:    *p =  *p           |  src;  break;
				case srcXor:   *p =  *p           ^  src;  break;
				case srcBic:   *p =  *p           & ~src;  break;
			}
			
			++p;
		}
		
		p += params.skip_bytes;
		
		v = (v + 1) & 0x7;
	}
	
	free( temp );
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
	
	SectRect( r,                     &clipRect, &clipRect );
	SectRect( &port.portBits.bounds, &clipRect, &clipRect );
	
	RgnHandle clipRgn = NULL;
	
	bool clipping_to_rect = port.clipRgn[0]->rgnSize <= sizeof (MacRegion);
	
	if ( !clipping_to_rect )
	{
		clipRgn = NewRgn();
		
		RectRgn( clipRgn, &clipRect );
		
		SectRgn( port.clipRgn, clipRgn, clipRgn );
		
		clipping_to_rect = clipRgn[0]->rgnSize <= sizeof (MacRegion);
		
		if ( clipping_to_rect )
		{
			clipRect = clipRgn[0]->rgnBBox;
			
			DisposeRgn( clipRgn );
			
			clipRgn = NULL;
		}
	}
	
	rectangular_op_params params;
	
	get_rectangular_op_params_for_rect( params, clipRect, !clipping_to_rect );
	
	params.pattern = &port.fillPat;
	params.origin_h = port.portBits.bounds.left;
	
	short patMode = patCopy;
	
	if ( verb == kQDGrafVerbErase )
	{
		if ( clipping_to_rect  &&  port.bkPat == White )
		{
			erase_rect( params );
			
			return;
		}
		
		port.fillPat = port.bkPat;
	}
	else if ( verb == kQDGrafVerbPaint )
	{
		if ( clipping_to_rect  &&  port.pnPat == Black )
		{
			paint_rect( params );
			
			return;
		}
		
		patMode      = port.pnMode;
		port.fillPat = port.pnPat;
	}
	else if ( verb == kQDGrafVerbInvert )
	{
		if ( clipping_to_rect )
		{
			invert_rect( params );
		
			return;
		}
		
		const QDGlobals& qd = get_QDGlobals();
		
		patMode      = patXor;
		port.fillPat = qd.black;
	}
	
	if ( clipping_to_rect  &&  patMode == patCopy )
	{
		fill_rect( params );
		
		return;
	}
	
	draw_rect( params, patMode, clipRect, clipRgn );
	
	if ( clipRgn != NULL )
	{
		DisposeRgn( clipRgn );
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
