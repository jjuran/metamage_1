/*
	stretch.cc
	----------
*/

#include "stretch.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


static const uint8_t stretched_2x_nibbles[] =
{
	0x00, 0x03, 0x0C, 0x0F,
	0x30, 0x33, 0x3C, 0x3F,
	0xC0, 0xC3, 0xCC, 0xCF,
	0xF0, 0xF3, 0xFC, 0xFF,
};

static
void blit_2x_stretched_segment( Ptr src, Ptr dst, short n_bytes )
{
	while ( n_bytes-- )
	{
		uint8_t octet = *src++;
		
		*dst++ = stretched_2x_nibbles[ octet >> 4 ];
		*dst++ = stretched_2x_nibbles[ octet & 15 ];
	}
}

static
uint8_t odd_numbered_bits( uint8_t x )
{
	uint8_t result = 0;
	uint8_t mask = 1;
	
	for ( int i = 0;  i < 4;  ++i )
	{
		x >>= 1;
		
		result |= x & mask;
		
		mask <<= 1;
	}
	
	return result;
}

static
void blit_2x_squished_segment( Ptr src, Ptr dst, short n_bytes )
{
	while ( n_bytes-- >= 0 )
	{
		uint8_t one = *src++;
		uint8_t two = *src++;
		
		uint8_t a = odd_numbered_bits( one );
		uint8_t b = odd_numbered_bits( two );
		
		*dst++ = (a << 4) | b;
	}
}

static
void stretch_2x( Ptr src, Ptr dst, short srcStride, short dstStride, short n )
{
	const short bytes_to_blit = dstStride / 2u;
	
	while ( n-- > 0 )
	{
		blit_2x_stretched_segment( src, dst, bytes_to_blit );
		
		dst += dstStride;
		
		blit_2x_stretched_segment( src, dst, bytes_to_blit );
		
		dst += dstStride;
		src += srcStride;
	}
}

static
void squish_2x( Ptr src, Ptr dst, short srcStride, short dstStride, short n )
{
	const short bytes_to_blit = dstStride;
	
	while ( n-- > 0 )
	{
		blit_2x_squished_segment( src, dst, bytes_to_blit );
		
		dst += dstStride;
		src += srcStride * 2;
	}
}

void stretch_bits( const BitMap&  srcBits,
                   const BitMap&  dstBits,
                   const Rect&    srcRect,
                   const Rect&    dstRect )
{
	const int srcWidth  = srcRect.right - srcRect.left;
	const int srcHeight = srcRect.bottom - srcRect.top;
	
	const int dstWidth  = dstRect.right - dstRect.left;
	const int dstHeight = dstRect.bottom - dstRect.top;
	
	const short srcTop = srcRect.top - srcBits.bounds.top;
	const short dstTop = dstRect.top - dstBits.bounds.top;
	
	const short srcLeft = srcRect.left - srcBits.bounds.left;
	const short dstLeft = dstRect.left - dstBits.bounds.left;
	
	Ptr src = srcBits.baseAddr + srcBits.rowBytes * srcTop + srcLeft / 8;
	Ptr dst = dstBits.baseAddr + dstBits.rowBytes * dstTop + dstLeft / 8;
	
	if ( dstWidth == 2 * srcWidth  &&  dstHeight == 2 * srcHeight )
	{
		stretch_2x( src, dst, srcBits.rowBytes, dstBits.rowBytes, srcHeight );
		return;
	}
	
	if ( srcWidth == 2 * dstWidth  &&  srcHeight == 2 * dstHeight )
	{
		squish_2x( src, dst, srcBits.rowBytes, dstBits.rowBytes, dstHeight );
		return;
	}
}
