/*
	Rects.cc
	--------
*/

#include "Rects.hh"

// Standard C
#include <stdlib.h>

// quickdraw
#include "qd/region_iterator.hh"

// ams-common
#include "callouts.hh"
#include "QDGlobals.hh"
#include "redraw_lock.hh"

// ams-qd
#include "color.hh"
#include "draw.hh"
#include "Regions.hh"
#include "segments_box.hh"


enum white_t { White };
enum black_t { Black };

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

static inline
bool erases_to_white( const GrafPort& port )
{
	return port.bkColor & Inverse  &&  port.bkPat == White;
}

static inline
bool paints_to_black( const GrafPort& port )
{
	return ! (port.fgColor & Inverse)  &&  port.pnPat == Black;
}

struct rectangular_op_params
{
	Rect rect;
	
	GrafPtr port;
	
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
                                                const Rect&             input_rect,
                                                bool                    clipping )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	params.port = thePort;
	
	const BitMap& portBits = thePort->portBits;
	
	const Rect& bounds = portBits.bounds;
	
	Rect rect;
	
	do_Rect_intersection( rect, bounds, input_rect );
	
	params.rect = rect;
	
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

static
Ptr draw_even_segment( Ptr      start,
                       short    n_bytes,
                       short    transfer_mode_AND_0x03,
                       uint8_t  pattern_sample )
{
	if ( transfer_mode_AND_0x03 == srcCopy )
	{
		fast_memset( start, pattern_sample, n_bytes );
		
		return start + n_bytes;
	}
	
	Ptr p = start;
	
	uint32_t src = pattern_sample << 8 | pattern_sample;
	
	src |= src << 16;
	
	while ( (uintptr_t) p & 0x3 )
	{
		p = draw_masked_byte( pattern_sample, 0xFF, p, transfer_mode_AND_0x03 );
		
		if ( --n_bytes == 0 )
		{
			return p;
		}
	}
	
	short n_longs = n_bytes / 4u;
	
	uint32_t* q = (uint32_t*) p;
	
	switch ( transfer_mode_AND_0x03 )
	{
		// Use src vs. pat modes because we stripped off the 8 bit
		// For srcBic, src is already negated.
		
		case srcCopy:  while ( --n_longs >= 0 )  *q++  =  src;  break;
		case srcOr:    while ( --n_longs >= 0 )  *q++ |=  src;  break;
		case srcXor:   while ( --n_longs >= 0 )  *q++ ^=  src;  break;
		case srcBic:   while ( --n_longs >= 0 )  *q++ &= ~src;  break;
	}
	
	p = (Ptr) q;
	
	n_bytes &= 0x3;
	
	while ( --n_bytes >= 0 )
	{
		p = draw_masked_byte( pattern_sample, 0xFF, p, transfer_mode_AND_0x03 );
	}
	
	return p;
}

static
Ptr draw_segment( Ptr      start,
                  short    n_pixels_skipped,
                  short    n_pixels_drawn,
                  short    transfer_mode_AND_0x03,
                  uint8_t  pattern_sample )
{
	Ptr p = start;
	
	if ( n_pixels_skipped )
	{
		uint8_t mask = (1 << (8 - n_pixels_skipped)) - 1;
		
		n_pixels_drawn -= 8 - n_pixels_skipped;
		
		if ( n_pixels_drawn < 0 )
		{
			n_pixels_skipped = -n_pixels_drawn;
			
			mask &= -(1 << n_pixels_skipped);
			
			n_pixels_drawn = 0;
		}
		
		p = draw_masked_byte( pattern_sample,
		                      mask,
		                      p,
		                      transfer_mode_AND_0x03 );
	}
	
	if ( n_pixels_drawn > 0 )
	{
		if ( short n_bytes = n_pixels_drawn / 8u )
		{
			p = draw_even_segment( p,
			                       n_bytes,
			                       transfer_mode_AND_0x03,
			                       pattern_sample );
		}
		
		n_pixels_drawn &= 0x7;
		
		if ( n_pixels_drawn > 0 )
		{
			n_pixels_skipped = 8 - n_pixels_drawn;
			
			const uint8_t mask = -(1 << n_pixels_skipped);
			
			p = draw_masked_byte( pattern_sample,
			                      mask,
			                      p,
			                      transfer_mode_AND_0x03 );
		}
	}
	
	return p;
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
		
		fast_memset( p, '\0', params.draw_bytes );
		
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
		
		fast_memset( p, 0xFF, params.draw_bytes );
		
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
	
	if ( params.left_mask == 0  &&  params.right_mask == 0 )
	{
		if ( params.skip_bytes == 0 )
		{
			fast_memnot( p, params.height * params.draw_bytes );
			return;
		}
	}
	
	for ( int i = params.height;  i > 0;  --i )
	{
		if ( params.left_mask )
		{
			char out = *p &  params.left_mask;
			char in = ~*p & ~params.left_mask;
			
			*p++ = out | in;
		}
		
		fast_memnot( p, params.draw_bytes );
		
		p += params.draw_bytes;
		
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
	
	QDGlobals& qd = get_QDGlobals();
	
	const short penWidth  = qd.thePort->pnSize.h;
	const short penHeight = qd.thePort->pnSize.v;
	
	const short w2 = penWidth  * 2;
	const short h2 = penHeight * 2;
	
	if ( rect->bottom - rect->top <= h2  ||  rect->right - rect->left <= w2 )
	{
		StdRect_patch( kQDGrafVerbPaint, rect );
		
		return;
	}
	
	Rect edge;
	
	*(Point*) &edge = *(Point*) rect;  // topLeft
	
	edge.right = rect->right;
	
	edge.bottom = edge.top + penHeight;
	
	StdRect_patch( kQDGrafVerbPaint, &edge );
	
	
	edge.top += penHeight;
	
	edge.right = edge.left + penWidth;
	
	edge.bottom = rect->bottom - penHeight;
	
	StdRect_patch( kQDGrafVerbPaint, &edge );
	
	
	edge.left  = rect->right - penWidth;
	edge.right = rect->right;
	
	StdRect_patch( kQDGrafVerbPaint, &edge );
	
	edge.left = rect->left;
	
	edge.top = edge.bottom;
	
	edge.bottom += penHeight;
	
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
		
		fast_memset( p, pat, params.draw_bytes );
		
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
                       short                         pattern_transfer_mode )
{
	Pattern& pattern = *params.pattern;
	
	const BitMap& portBits = params.port->portBits;
	
	const Rect& bounds = portBits.bounds;
	
	const Rect& rect = params.rect;
	
	const short top    = params.topLeft.v;
	const short bottom = params.topLeft.v + params.height;
	
	short pat_v = top & 0x7;
	
	Ptr rowBase = portBits.baseAddr + (top - bounds.top) * portBits.rowBytes;
	
	for ( int i = top;  i < bottom;  ++i )
	{
		const uint8_t pat = pattern.pat[ pat_v ];
		
		Ptr start = rowBase + (rect.left - bounds.left) / 8u;
		
		const short n_pixels_skipped = (rect.left - bounds.left) & 0x7;
		const short n_pixels_drawn   = rect.right - rect.left;
		
		draw_segment( start,
		              n_pixels_skipped,
		              n_pixels_drawn,
		              pattern_transfer_mode & 0x03,
		              pat );
		
		pat_v = (pat_v + 1) & 0x7;
		
		rowBase += portBits.rowBytes;
	}
}

static
void draw_region( const rectangular_op_params&  params,
                  short                         pattern_transfer_mode,
                  RgnHandle                     region )
{
	const short* extent = (short*) (*region + 1);
	
	segments_box segments( region[0]->rgnSize );
	
	quickdraw::region_iterator it( segments, extent );
	
	const BitMap& portBits = params.port->portBits;
	
	const Rect& bounds = portBits.bounds;
	
	while ( const quickdraw::region_band* band = it.next() )
	{
		const short v0 = band->v0;
		const short v1 = band->v1;
		
		Ptr rowBase = portBits.baseAddr + (v0 - bounds.top) * portBits.rowBytes;
		
		for ( short v = v0;  v < v1;  ++v )
		{
			const uint8_t pat = params.pattern->pat[ v & 0x7 ];
			
			const short* it  = band->h_begin;
			const short* end = band->h_end;
			
			while ( it < end )
			{
				const short h0 = *it++;
				const short h1 = *it++;
				
				Ptr start = rowBase + (h0 - bounds.left) / 8u;
				
				const short n_pixels_skipped = (h0 - bounds.left) & 0x7;
				const short n_pixels_drawn   =  h1 - h0;
				
				draw_segment( start,
				              n_pixels_skipped,
				              n_pixels_drawn,
				              pattern_transfer_mode & 0x03,
				              pat );
			}
			
			rowBase += portBits.rowBytes;
		}
	}
}

static RgnHandle clipRgn = NewRgn_patch();

pascal void StdRect_patch( signed char verb, const Rect* r )
{
	GrafPort& port = **get_addrof_thePort();
	
	redraw_lock lock( port.portBits, *r );
	
	if ( verb == kQDGrafVerbFrame )
	{
		if ( RgnHandle rgnSave = (RgnHandle) port.rgnSave )
		{
			RgnHandle rgn = clipRgn;  // Reuse the existing region
			
			RectRgn( rgn, r );
			
			UnionRgn( rgn, rgnSave, rgnSave );
		}
		
		if ( port.pnVis >= 0 )
		{
			frame_rect( r );
		}
		
		return;
	}
	
	if ( port.pnVis < 0 )
	{
		return;
	}
	
	// This initializes clipRgn by calling RectRgn().
	get_refined_clip_region( port, *r, clipRgn );
	
	const Rect clipRect = clipRgn[0]->rgnBBox;
	
	bool clipping_to_rect = clipRgn[0]->rgnSize <= sizeof (MacRegion);
	
	rectangular_op_params params;
	
	get_rectangular_op_params_for_rect( params, clipRect, !clipping_to_rect );
	
	params.pattern = &port.fillPat;
	params.origin_h = port.portBits.bounds.left;
	
	short patMode = patCopy;
	
	if ( verb == kQDGrafVerbErase )
	{
		if ( clipping_to_rect  &&  erases_to_white( port ) )
		{
			erase_rect( params );
			
			return;
		}
		
		port.fillPat = port.bkPat;
		
		if ( ! (port.bkColor & Inverse) )
		{
			patMode ^= 4;  // non-white background; negate the pattern
		}
	}
	else if ( verb == kQDGrafVerbPaint )
	{
		const short mode = port.pnMode & 0x7;
		
		if ( clipping_to_rect  &&  mode <= srcOr  &&  paints_to_black( port ) )
		{
			// patCopy or patOr -- use optimized paint routine
			paint_rect( params );
			
			return;
		}
		
		patMode      = port.pnMode;
		port.fillPat = port.pnPat;
		
		if ( port.fgColor & Inverse )
		{
			patMode ^= 4;  // white foreground; negate the pattern
		}
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
	
	Pattern& pattern = *params.pattern;
	
	if ( const bool negated = patMode & 0x04 )
	{
		patMode -= 0x04;
		
		for ( int i = 0;  i < 8;  ++i )
		{
			pattern.pat[ i ] = ~pattern.pat[ i ];
		}
	}
	
	if ( const short h = params.origin_h & 0x07 )
	{
		for ( int i = 0;  i < 8;  ++i )
		{
			pattern.pat[ i ] = pattern.pat[ i ] <<      h
			                 | pattern.pat[ i ] >> (8 - h);
		}
	}
	
	if ( ! clipping_to_rect )
	{
		draw_region( params, patMode, clipRgn );
	}
	else
	{
		draw_rect( params, patMode );
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
