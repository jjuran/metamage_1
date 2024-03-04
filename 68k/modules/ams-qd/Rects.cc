/*
	Rects.cc
	--------
*/

#include "Rects.hh"

// Mac OS
#ifndef __FIXMATH__
#include <FixMath.h>
#endif

// Standard C
#include <stdlib.h>

// quickdraw
#include "qd/region_iterator.hh"

// ams-common
#include "callouts.hh"
#include "math.hh"
#include "QDGlobals.hh"
#include "redraw_lock.hh"

// ams-qd
#include "color.hh"
#include "draw.hh"
#include "Regions.hh"
#include "segments_box.hh"
#include "utility_region.hh"


#pragma exceptions off


struct rectangular_op_params
{
	Rect rect;
	
	GrafPtr port;
	
	Point topLeft;
	
	Ptr start;
	
	uint16_t height;
	
	uint16_t draw_bytes;
	uint16_t skip_bytes;
	
	Byte left_mask;
	Byte right_mask;
	
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
	do_Rect_intersection( rect, thePort->portRect, rect );
	
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
	
	const Byte left_mask  = (left  & 0x7) ? ~((1 << 8 - (left  & 0x7)) - 1) : 0;
	const Byte right_mask = (right & 0x7) ?   (1 << 8 - (right & 0x7)) - 1  : 0;
	
	const uint32_t rowBytes = portBits.rowBytes;
	
	Ptr baseAddr = portBits.baseAddr;
	
	params.topLeft    = *(Point*) &rect;
	params.start      = baseAddr + mulu_w( rowBytes, top ) + outer_left_bytes;
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
void draw_sector( Ptr    src,
                  short  src_height,
                  short  src_v_index,
                  Ptr    dst,
                  short  dst_height,
                  short  dst_stride,
                  short  n_pixels_skipped,
                  short  n_pixels_drawn,
                  short  transfer_mode_AND_0x03 )
{
	if ( n_pixels_skipped )
	{
		Byte mask = (1 << (8 - n_pixels_skipped)) - 1;
		
		n_pixels_drawn -= 8 - n_pixels_skipped;
		
		if ( n_pixels_drawn < 0 )
		{
			n_pixels_skipped = -n_pixels_drawn;
			
			mask &= -(1 << n_pixels_skipped);
			
			n_pixels_drawn = 0;
		}
		
		draw_masked_column( src,
		                    src_height,
		                    src_v_index,
		                    dst,
		                    dst_height,
		                    dst_stride,
		                    mask,
		                    transfer_mode_AND_0x03 );
		
		++dst;
	}
	
	if ( n_pixels_drawn > 0 )
	{
		if ( short n_bytes = n_pixels_drawn / 8u )
		{
			draw_bytes( src, 0, src_v_index,
			            dst, n_bytes, dst_height, dst_stride,
			            transfer_mode_AND_0x03 );
			
			dst += n_bytes;
		}
		
		n_pixels_drawn &= 0x7;
		
		if ( n_pixels_drawn > 0 )
		{
			n_pixels_skipped = 8 - n_pixels_drawn;
			
			const Byte mask = -(1 << n_pixels_skipped);
			
			draw_masked_column( src,
			                    src_height,
			                    src_v_index,
			                    dst,
			                    dst_height,
			                    dst_stride,
			                    mask,
			                    transfer_mode_AND_0x03 );
		}
	}
}

static inline
void draw_sector( const Pattern&  pattern,
                  short           pat_v_index,
                  Ptr             dst,
                  short           dst_height,
                  short           dst_stride,
                  short           n_pixels_skipped,
                  short           n_pixels_drawn,
                  short           transfer_mode_AND_0x03 )
{
	Ptr src = (Ptr) pattern.pat;
	
	draw_sector( src, sizeof pattern, pat_v_index,
	             dst, dst_height, dst_stride,
	             n_pixels_skipped,
	             n_pixels_drawn,
	             transfer_mode_AND_0x03 );
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
		StdRect( kQDGrafVerbPaint, rect );
		return;
	}
	
	Rect edge;
	
	*(Point*) &edge = *(Point*) rect;  // topLeft
	
	edge.right = rect->right;
	
	edge.bottom = edge.top + penHeight;
	
	StdRect( kQDGrafVerbPaint, &edge );
	
	
	edge.top += penHeight;
	
	edge.right = edge.left + penWidth;
	
	edge.bottom = rect->bottom - penHeight;
	
	StdRect( kQDGrafVerbPaint, &edge );
	
	
	edge.left  = rect->right - penWidth;
	edge.right = rect->right;
	
	StdRect( kQDGrafVerbPaint, &edge );
	
	edge.left = rect->left;
	
	edge.top = edge.bottom;
	
	edge.bottom += penHeight;
	
	StdRect( kQDGrafVerbPaint, &edge );
}

static
void draw_rect( const rectangular_op_params&  params,
                short                         transfer_mode_AND_0x03 )
{
	Pattern& pattern = *params.pattern;
	
	const BitMap& portBits = params.port->portBits;
	
	const Rect& bounds = portBits.bounds;
	
	const Rect& rect = params.rect;
	
	const short top    = params.topLeft.v;
	const short bottom = params.topLeft.v + params.height;
	
	short pat_v = top & 0x7;
	
	const uint16_t n_rows_skipped = top - bounds.top;
	
	Ptr rowBase = portBits.baseAddr
	            + mulu_w( portBits.rowBytes, n_rows_skipped )
	            + (rect.left - bounds.left) / 8u;
	
	const short n_pixels_skipped = (rect.left - bounds.left) & 0x7;
	const short n_pixels_drawn   = rect.right - rect.left;
	
	draw_sector( pattern, pat_v,
	             rowBase,
	             bottom - top,
	             portBits.rowBytes,
		         n_pixels_skipped,
		         n_pixels_drawn,
		         transfer_mode_AND_0x03 );
}

static
void draw_region( const rectangular_op_params&  params,
                  short                         transfer_mode_AND_0x03,
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
		
		const uint16_t n_rows_skipped = v0 - bounds.top;
		
		Ptr rowBase = portBits.baseAddr
		            + mulu_w( portBits.rowBytes, n_rows_skipped );
		
		short v = v0;
		
		short height = v1 - v0;
		
		const Byte pat = params.pattern->pat[ v & 0x7 ];
		
		const short* it  = band->h_begin;
		const short* end = band->h_end;
		
		while ( it < end )
		{
			const short h0 = *it++;
			const short h1 = *it++;
			
			Ptr start = rowBase + (h0 - bounds.left) / 8u;
			
			const short n_pixels_skipped = (h0 - bounds.left) & 0x7;
			const short n_pixels_drawn   =  h1 - h0;
			
			draw_sector( *params.pattern, v & 0x7,
			             start, height, portBits.rowBytes,
			             n_pixels_skipped,
			             n_pixels_drawn,
			             transfer_mode_AND_0x03 );
		}
		
		rowBase += portBits.rowBytes * height;
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
			UnionRgn( rectangular_utility_region( *r ), rgnSave, rgnSave );
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
		port.fillPat = port.bkPat;
		
		if ( ! (port.bkColor & Inverse) )
		{
			patMode ^= 4;  // non-white background; negate the pattern
		}
	}
	else if ( verb == kQDGrafVerbPaint )
	{
		patMode      = port.pnMode;
		port.fillPat = port.pnPat;
		
		if ( port.fgColor & Inverse )
		{
			patMode ^= 4;  // white foreground; negate the pattern
		}
	}
	else if ( verb == kQDGrafVerbInvert )
	{
		const QDGlobals& qd = get_QDGlobals();
		
		patMode      = patXor;
		port.fillPat = qd.black;
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
		draw_region( params, patMode & 0x03, clipRgn );
	}
	else
	{
		draw_rect( params, patMode & 0x03 );
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
	if ( (dv < 0 ? -rect->top - dv : rect->bottom + dv) > 32767 )
	{
		return;
	}
	
	if ( (dh < 0 ? -rect->left - dh : rect->right + dh) > 32767 )
	{
		return;
	}
	
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
	
	if ( EmptyRect( c ) )
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

pascal void PtToAngle_patch( const Rect* rect, Point pt, short* result )
{
	const UInt16 height = rect->bottom - rect->top;
	const UInt16 width  = rect->right - rect->left;
	
	const short cv = rect->bottom + rect->top >> 1;  // rect center V
	const short ch = rect->right + rect->left >> 1;  // rect center H
	
	const UInt16 rise = pt.v - cv;
	const UInt16 run  = pt.h - ch;
	
	short angle = 0;
	
	if ( run == 0 )
	{
		if ( pt.v > cv )
		{
			angle = 180;
		}
	}
	else
	{
		if ( rise == 0 )
		{
			angle = 90;
		}
		else
		{
			const uint32_t run_x_height = mulu_w( run, height );
			const uint32_t rise_x_width = mulu_w( rise, width );
			
			angle = AngleFromSlope( FixRatio( run_x_height, rise_x_width ) );
		}
		
		if ( pt.h < ch )
		{
			angle += 180;
		}
	}
	
	*result = angle;
}

pascal unsigned char EmptyRect_patch( const Rect* rect )
{
	return rect->top >= rect->bottom  ||  rect->left >= rect->right;
}
