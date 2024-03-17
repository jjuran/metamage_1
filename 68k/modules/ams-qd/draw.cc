/*
	draw.cc
	-------
*/

#include "draw.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


Ptr ScrnBase : 0x0824;


void get_refined_clip_region( const GrafPort&  port,
                              const Rect&      dstRect,
                              RgnHandle        result )
{
	Rect clipRect = port.clipRgn[0]->rgnBBox;
	
	SectRect( &dstRect,              &clipRect, &clipRect );
	SectRect( &port.portBits.bounds, &clipRect, &clipRect );
	
	RectRgn( result, &clipRect );
	
	if ( port.portBits.baseAddr == ScrnBase )
	{
		SectRgn( port.visRgn, result, result );
	}
	
	SectRgn( port.clipRgn, result, result );
}

Ptr draw_masked_byte( Byte   byte,
                      Byte   mask,
                      Ptr    dst,
                      short  transfer_mode_AND_0x07 )
{
	if ( transfer_mode_AND_0x07 >= 0x04 )
	{
		byte = ~byte;
		
		transfer_mode_AND_0x07 -= 0x04;
	}
	
	byte &= mask;
	
	switch ( transfer_mode_AND_0x07 )
	{
		// Use src vs. pat modes because we stripped off the 8 bit
		
		case srcCopy:  *dst   &= ~mask;  // fall through
		case srcOr:    *dst++ |=  byte;  break;
		case srcXor:   *dst++ ^=  byte;  break;
		case srcBic:   *dst++ &= ~byte;  break;
	}
	
	return dst;
}

void blit_masked_column( Ptr    src,
                         short  src_stride,
                         Ptr    dst,
                         short  dst_stride,
                         short  height,
                         Byte   mask,
                         short  transfer_mode_AND_0x07 )
{
	while ( height-- )
	{
		draw_masked_byte( *src, mask, dst, transfer_mode_AND_0x07 );
		
		src += src_stride;
		dst += dst_stride;
	}
}

void draw_masked_column( Ptr    src,
                         short  src_height,
                         short  src_index,
                         Ptr    dst,
                         short  dst_height,
                         short  dst_stride,
                         Byte   mask,
                         short  transfer_mode_AND_0x07 )
{
	Ptr p = src + src_index;
	
	while ( dst_height-- )
	{
		draw_masked_byte( *p++, mask, dst, transfer_mode_AND_0x07 );
		
		dst += dst_stride;
		
		if ( ++src_index == src_height )
		{
			src_index = 0;
			
			p = src;
		}
	}
}
