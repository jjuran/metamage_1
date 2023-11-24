/*
	stretch.cc
	----------
*/

#include "stretch.hh"

// Mac OS
#ifndef __FIXMATH__
#include <FixMath.h>
#endif
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// ams-common
#include "math.hh"


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
	
	const int srcLeft = srcRect.left - srcBits.bounds.left;
	const int dstLeft = dstRect.left - dstBits.bounds.left;
	
	const short srcRowBytes = srcBits.rowBytes;
	const short dstRowBytes = dstBits.rowBytes;
	
	Ptr src = srcBits.baseAddr + mulu_w( srcRowBytes, srcTop ) + srcLeft / 8u;
	Ptr dst = dstBits.baseAddr + mulu_w( dstRowBytes, dstTop ) + dstLeft / 8u;
	
	const bool aligned = ! ((srcLeft | dstLeft | srcWidth | dstWidth) & 7);
	
	if ( aligned  &&  dstWidth == 2 * srcWidth  &&  dstHeight == 2 * srcHeight )
	{
		stretch_2x( src, dst, srcRowBytes, dstRowBytes, srcHeight );
		return;
	}
	
	if ( aligned  &&  srcWidth == 2 * dstWidth  &&  srcHeight == 2 * dstHeight )
	{
		squish_2x( src, dst, srcRowBytes, dstRowBytes, dstHeight );
		return;
	}
	
	const Fixed x_sampling_factor = FixRatio( srcWidth,  dstWidth  );
	const Fixed y_sampling_factor = FixRatio( srcHeight, dstHeight );
	
	for ( short y = 0;  y < dstHeight;  ++y )
	{
		const short src_row = srcTop + fixmulu_w( y, y_sampling_factor );
		
		src = srcBits.baseAddr + srcBits.rowBytes * src_row;
		
		int16_t index = 0;
		int16_t shift = 8;
		
		for ( uint16_t x = 0;  x < dstWidth;  ++x )
		{
			if ( --shift < 0 )
			{
				shift = 7;
				
				++index;
			}
			
			uint32_t src_col = srcLeft + fixmulu_w( x, x_sampling_factor );
			
			const Byte byte = src[ src_col / 8u ];
			const int bitnum = 7 - src_col % 8u;
			
			const bool pixel = byte & (1 << bitnum);
			
			dst[ index ] |= pixel << shift;
		}
		
		dst += dstBits.rowBytes;
	}
}
