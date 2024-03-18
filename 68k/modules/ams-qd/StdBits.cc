/*
	StdBits.cc
	----------
*/

#include "StdBits.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// quickdraw
#include "qd/region_iterator.hh"

// ams-common
#include "callouts.hh"
#include "math.hh"
#include "QDGlobals.hh"
#include "redraw_lock.hh"
#include "scoped_zone.hh"

// ams-qd
#include "draw.hh"
#include "Picture-bits.hh"
#include "segments_box.hh"
#include "stretch.hh"


#pragma exceptions off


static inline
bool byte_aligned( short dstSkip, short width )
{
	return dstSkip == 0  &&  width % 8u == 0;
}

static
void blit_sector( Ptr    src,
                  short  src_stride,
                  Ptr    dst,
                  short  dst_stride,
                  short  height,
                  short  n_pixels_skipped,
                  short  n_pixels_drawn,
                  short  transfer_mode_AND_0x07 )
{
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
		
		blit_masked_column( src, src_stride,
		                    dst, dst_stride,
		                    height,
		                    mask,
		                    transfer_mode_AND_0x07 );
		
		++src;
		++dst;
	}
	
	if ( n_pixels_drawn == 0 )
	{
		return;
	}
	
	if ( short n_bytes = n_pixels_drawn / 8u )
	{
		blit_bytes( src, src_stride,
		            dst, dst_stride,
		            n_bytes,
		            height,
		            transfer_mode_AND_0x07 );
		
		src += n_bytes;
		dst += n_bytes;
	}
	
	n_pixels_drawn &= 0x7;
	
	if ( n_pixels_drawn > 0 )
	{
		n_pixels_skipped = 8 - n_pixels_drawn;
		
		const uint8_t mask = -(1 << n_pixels_skipped);
		
		blit_masked_column( src, src_stride,
		                    dst, dst_stride,
		                    height,
		                    mask,
		                    transfer_mode_AND_0x07 );
	}
}

static
void blit_masked_bits( const BitMap&  srcBits,
                       const BitMap&  dstBits,
                       short          dh,
                       short          dv,
                       short          mode,
                       RgnHandle      maskRgn )
{
	const short srcVOffset = srcBits.bounds.top  + dv;
	const short srcHOffset = srcBits.bounds.left + dh;
	
	const short dstVOffset = dstBits.bounds.top;
	const short dstHOffset = dstBits.bounds.left;
	
	const short* extent = (short*) (*maskRgn + 1);
	
	segments_box segments( maskRgn[0]->rgnSize );
	
	quickdraw::region_iterator it( segments, extent );
	
	while ( const quickdraw::region_band* band = it.next() )
	{
		const short v0 = band->v0;
		const short v1 = band->v1;
		
		Ptr p = srcBits.baseAddr + mulu_w( srcBits.rowBytes, v0 - srcVOffset );
		Ptr q = dstBits.baseAddr + mulu_w( dstBits.rowBytes, v0 - dstVOffset );
		
		short height = v1 - v0;
		
		const short* it  = band->h_begin;
		const short* end = band->h_end;
		
		while ( it < end )
		{
			const short h0 = *it++;
			const short h1 = *it++;
			
			Ptr src = p + (h0 - srcHOffset) / 8u;
			Ptr dst = q + (h0 - dstHOffset) / 8u;
			
			const short n_dst_pixels_skipped = h0 - dstHOffset & 7;
			const short n_pixels_drawn       = h1 - h0;
			
			blit_sector( src,
			             srcBits.rowBytes,
			             dst,
			             dstBits.rowBytes,
			             height,
			             n_dst_pixels_skipped,
			             n_pixels_drawn,
			             mode );
		}
	}
}

pascal void StdBits_patch( const BitMap*  srcBits,
                           const Rect*    srcRect,
                           const Rect*    dstRect,
                           short          mode,
                           MacRegion**    maskRgn )
{
	const int srcWidth  = srcRect->right - srcRect->left;
	const int srcHeight = srcRect->bottom - srcRect->top;
	
	const int dstWidth  = dstRect->right - dstRect->left;
	const int dstHeight = dstRect->bottom - dstRect->top;
	
	if ( srcWidth <= 0  ||  srcHeight <= 0 )
	{
		return;
	}
	
	if ( dstWidth <= 0  ||  dstHeight <= 0 )
	{
		return;
	}
	
	GrafPort& port = **get_addrof_thePort();
	
	if ( Handle h = port.picSave )
	{
		save_bits_to_picture( h, srcBits, srcRect, dstRect, mode, maskRgn );
	}
	
	if ( port.pnVis < 0 )
	{
		return;
	}
	
	const bool congruent  = dstWidth == srcWidth  &&  dstHeight == srcHeight;
	const bool stretching = ! congruent;
	
	static RgnHandle clipRgn = (scoped_zone(), NewRgn());
	
	BitMap stretched_bits;
	
	if ( stretching )
	{
		static Handle buffer = (scoped_zone(), NewHandle( 0 ));
		
		const short rowBytes = (dstWidth + 15) / 16u * 2;
		
		Size size = mulu_w( rowBytes, dstHeight );
		
		SetHandleSize( buffer, size );
		
		fast_memset( *buffer, '\0', size );
		
		const Rect bounds = { 0, 0, dstHeight, dstWidth };
		
		stretched_bits.baseAddr = *buffer;  // no locking needed yet
		stretched_bits.rowBytes = rowBytes;
		stretched_bits.bounds   = bounds;
		
		stretch_bits( *srcBits, stretched_bits, *srcRect, bounds );
		
		srcBits = &stretched_bits;
		srcRect = &stretched_bits.bounds;
	}
	
	Rect clippedSrcRect = *srcRect;
	Rect clippedDstRect = *dstRect;
	
	SectRect( &srcBits->bounds, &clippedSrcRect, &clippedSrcRect );
	
	clippedDstRect.top  += clippedSrcRect.top  - srcRect->top;
	clippedDstRect.left += clippedSrcRect.left - srcRect->left;
	
	clippedDstRect.right  += clippedSrcRect.right  - srcRect->right;
	clippedDstRect.bottom += clippedSrcRect.bottom - srcRect->bottom;
	
	srcRect = &clippedSrcRect;
	dstRect = &clippedDstRect;
	
	get_refined_clip_region( port, *dstRect, clipRgn );
	
	if ( maskRgn )
	{
		SectRgn( maskRgn, clipRgn, clipRgn );
	}
	
	const bool clipping_to_rect = clipRgn[0]->rgnSize <= sizeof (MacRegion);
	
	const Rect clipRect = clipRgn[0]->rgnBBox;
	
	uint16_t n_rows = clipRect.bottom - clipRect.top;
	uint16_t width  = clipRect.right - clipRect.left;
	
	if ( width == 0  || n_rows == 0 )
	{
		return;
	}
	
	redraw_lock lock( port.portBits, *dstRect, *srcBits, *srcRect );
	
	const uint16_t clippedTop  = clipRect.top  - dstRect->top;
	const uint16_t clippedLeft = clipRect.left - dstRect->left;
	
	QDGlobals& qd = get_QDGlobals();
	
	const BitMap* const dstBits = &qd.thePort->portBits;
	
	const short srcTop = srcRect->top + clippedTop - srcBits->bounds.top;
	const short dstTop = dstRect->top + clippedTop - dstBits->bounds.top;
	
	const short srcLeft = srcRect->left + clippedLeft - srcBits->bounds.left;
	const short dstLeft = dstRect->left + clippedLeft - dstBits->bounds.left;
	
	short srcRowBytes = srcBits->rowBytes;
	short dstRowBytes = dstBits->rowBytes;
	
	Ptr src = srcBits->baseAddr;
	Ptr dst = dstBits->baseAddr;
	
	src += srcTop * srcRowBytes;
	dst += dstTop * dstRowBytes;
	
	src += srcLeft / 8u;
	dst += dstLeft / 8u;
	
	const short srcSkip = srcLeft & 0x7;
	const short dstSkip = dstLeft & 0x7;
	
	bool buffering = false;
	
	if ( srcSkip != dstSkip )
	{
		buffering = true;
	}
	else if ( const bool same_bitmap = srcBits->baseAddr == dstBits->baseAddr )
	{
		// Overlapping graphics
		
		buffering = dstTop  < srcTop  + n_rows  &&
		            srcTop  < dstTop  + n_rows  &&
		            dstLeft < srcLeft + width   &&
		            srcLeft < dstLeft + width;
	}
	
	BitMap buffered_bits;
	Rect   buffered_bits_rect;
	
	if ( buffering )
	{
		static Handle buffer = (scoped_zone(), NewHandle( 0 ));
		
		const int rowBytes = width / 16u * 2 + 4;  // include a 2-byte margin
		
		Size size = mulu_w( rowBytes, n_rows );
		
		SetHandleSize( buffer, size );
		
		Ptr tmp = *buffer;
		
		fast_memset( tmp, '\0', size );
		
		int skip_delta = srcSkip - dstSkip;
		
		Ptr tmp1 = tmp + (skip_delta < 0);
		
		blit_bytes( src, srcRowBytes, tmp1, rowBytes, rowBytes - 1, n_rows );
		
		fast_lshift( tmp, size, skip_delta & 0x7 );
		
		src         = tmp;
		srcRowBytes = rowBytes;
		
		const Rect bounds = { 0, 0, n_rows, rowBytes };
		
		buffered_bits_rect = bounds;
		
		buffered_bits_rect.right = width;
		
		OffsetRect( &buffered_bits_rect, dstSkip - clippedLeft, -clippedTop );
		
		buffered_bits.baseAddr = tmp;  // no locking needed yet
		buffered_bits.rowBytes = rowBytes;
		buffered_bits.bounds   = bounds;
		
		srcBits = &buffered_bits;
		srcRect = &buffered_bits_rect;
	}
	
	if ( clipping_to_rect )
	{
		if ( byte_aligned( dstSkip, width ) )
		{
			width /= 8;
			
			blit_bytes( src, srcRowBytes, dst, dstRowBytes, width, n_rows, mode );
		}
		else
		{
			blit_sector( src,
			             srcRowBytes,
			             dst,
			             dstRowBytes,
			             n_rows,
			             dstSkip,
			             width,
			             mode );
		}
	}
	else
	{
		{
			blit_masked_bits( *srcBits,
			                  *dstBits,
			                  dstRect->left - srcRect->left,
			                  dstRect->top  - srcRect->top,
			                  mode,
			                  clipRgn );
		}
	}
}
