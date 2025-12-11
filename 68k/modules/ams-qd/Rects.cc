/*
	Rects.cc
	--------
*/

#include "Rects.hh"

// quickdraw
#include "qd/region_iterator.hh"

// ams-common
#include "callouts.hh"
#include "math.hh"
#include "QDGlobals.hh"
#include "redraw_lock.hh"
#include "utility_region.hh"

// ams-qd
#include "color.hh"
#include "draw.hh"
#include "GrafProcs.hh"
#include "Regions.hh"
#include "segments_box.hh"


#pragma exceptions off


struct rectangular_op_params
{
	Rect rect;
	
	GrafPtr port;
	
	uint16_t rowBytes;
};

static inline
void get_rectangular_op_params( rectangular_op_params&  params,
                                const Rect&             rect )
{
	GrafPtr port = get_thePort();
	
	params.port = port;
	params.rect = rect;
	
	params.rowBytes = port->portBits.rowBytes;
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

static
void frame_rect( const Rect* rect )
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

static
void draw_rect( const rectangular_op_params&  params,
                short                         transfer_mode_AND_0x03 )
{
	const BitMap& portBits = params.port->portBits;
	
	const Rect& bounds = portBits.bounds;
	
	const Rect& rect = params.rect;
	
	const uint16_t n_rows_skipped = params.rect.top - bounds.top;
	
	UInt16 rowBytes = params.rowBytes;
	
	Ptr rowBase = portBits.baseAddr
	            + mulu_w( rowBytes, n_rows_skipped )
	            + (rect.left - bounds.left) / 8u;
	
	const short n_pixels_skipped = (rect.left - bounds.left) & 0x7;
	const short n_pixels_drawn   = rect.right - rect.left;
	
	Pattern& pattern = params.port->fillPat;
	
	short pat_v = params.rect.top & 0x7;
	
	draw_sector( pattern, pat_v,
	             rowBase,
	             rect.bottom - rect.top,
	             rowBytes,
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
	
	UInt16 rowBytes = params.rowBytes;
	
	while ( const quickdraw::region_band* band = it.next() )
	{
		const short v0 = band->v0;
		const short v1 = band->v1;
		
		const uint16_t n_rows_skipped = v0 - bounds.top;
		
		Ptr rowBase = portBits.baseAddr
		            + mulu_w( rowBytes, n_rows_skipped );
		
		short height = v1 - v0;
		
		const short* it  = band->h_begin;
		const short* end = band->h_end;
		
		while ( it < end )
		{
			const short h0 = *it++;
			const short h1 = *it++;
			
			Ptr start = rowBase + (h0 - bounds.left) / 8u;
			
			const short n_pixels_skipped = (h0 - bounds.left) & 0x7;
			const short n_pixels_drawn   =  h1 - h0;
			
			draw_sector( params.port->fillPat, v0 & 0x7,
			             start, height, rowBytes,
			             n_pixels_skipped,
			             n_pixels_drawn,
			             transfer_mode_AND_0x03 );
		}
	}
}

static RgnHandle clipRgn = NewRgn_patch();

pascal void StdRect_patch( signed char verb, const Rect* r )
{
	GrafPort& port = *get_thePort();
	
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
	
	Rect clipRect;
	
	SectRect( r, &port.portRect, &clipRect );
	
	// This initializes clipRgn by calling RectRgn().
	get_refined_clip_region( port, clipRect, clipRgn );
	
	if ( EmptyRgn( clipRgn ) )
	{
		return;
	}
	
	bool clipping_to_rect = clipRgn[0]->rgnSize <= sizeof (MacRegion);
	
	rectangular_op_params params;
	
	get_rectangular_op_params( params, clipRgn[0]->rgnBBox );
	
	short origin_h = port.portBits.bounds.left;
	
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
	else if ( verb == kQDGrafVerbFill )
	{
		long fg_inverted = port.fgColor & Inverse;
		long bk_inverted = port.bkColor & Inverse;
		
		if ( fg_inverted == bk_inverted )
		{
			/*
				Either black/color on black/color, or white on white.
			*/
			
			const QDGlobals& qd = get_QDGlobals();
			
			patMode      = patCopy;
			port.fillPat = qd.black;
		}
		
		if ( fg_inverted )
		{
			/*
				Either white on black or white on white.
			*/
			
			patMode ^= 4;  // negate the pattern
		}
	}
	
	Pattern& pattern = port.fillPat;
	
	if ( const bool negated = patMode & 0x04 )
	{
		patMode -= 0x04;
		
		for ( int i = 0;  i < 8;  ++i )
		{
			pattern.pat[ i ] = ~pattern.pat[ i ];
		}
	}
	
	if ( const short h = origin_h & 0x07 )
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
	GrafPort& port = *get_thePort();
	
	CHECK_CUSTOM_GRAFPROC( port, rectProc );
	
	GRAFPROC_RECT( port )( kQDGrafVerbErase, rect );
}

pascal void PaintRect_patch( const Rect* rect )
{
	GrafPort& port = *get_thePort();
	
	CHECK_CUSTOM_GRAFPROC( port, rectProc );
	
	GRAFPROC_RECT( port )( kQDGrafVerbPaint, rect );
}

pascal void InverRect_patch( const Rect* rect )
{
	GrafPort& port = *get_thePort();
	
	CHECK_CUSTOM_GRAFPROC( port, rectProc );
	
	GRAFPROC_RECT( port )( kQDGrafVerbInvert, rect );
}

pascal void FrameRect_patch( const Rect* rect )
{
	GrafPort& port = *get_thePort();
	
	CHECK_CUSTOM_GRAFPROC( port, rectProc );
	
	GRAFPROC_RECT( port )( kQDGrafVerbFrame, rect );
}

pascal void FillRect_patch( const Rect* rect, const Pattern* pattern )
{
	GrafPort& port = *get_thePort();
	
	port.fillPat = *pattern;
	
	CHECK_CUSTOM_GRAFPROC( port, rectProc );
	
	GRAFPROC_RECT( port )( kQDGrafVerbFill, rect );
}
