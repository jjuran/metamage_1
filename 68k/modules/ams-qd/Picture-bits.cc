/*
	Picture-bits.cc
	---------------
*/

#include "Picture-bits.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// Standard C
#include <stdlib.h>  // for alloca()

// log-of-war
#include "logofwar/report.hh"

// quickdraw
#include "qd/pack_bits.hh"

// ams-common
#include "callouts.hh"

// ams-qd
#include "palette.hh"
#include "Pictures-common.hh"
#include "transcode.hh"


#pragma exceptions off


struct Picture_PixMap
{
//	short rowBytes;
//	Rect  bounds;
	short pmVersion;
	short packType;
	long  packSize;
	Fixed hRes;
	Fixed vRes;
	short pixelType;
	short pixelSize;
	short cmpCount;
	short cmpSize;
};

const Byte* draw_bits( const Byte* p, bool has_region )
{
	Picture_PixMap pixmap;
	
	BitMap bitmap;
	
	Byte polarized_palette[ 256 ];
	
	short rowBytes = read_word( p );
	
	bitmap.bounds = read_Rect( p );
	
	const bool color = rowBytes < 0;
	
	if ( color )
	{
		rowBytes &= 0x3FFF;
		
		fast_memcpy( &pixmap, p, sizeof (Picture_PixMap) );
		
		p += sizeof (Picture_PixMap) + 18;  // 24 + 18 = 42
		
		UInt16 n_colors = read_word( p ) + 1;
		
		const ColorSpec* color_palette = (const ColorSpec*) p;
		
		polarize_palette( color_palette, n_colors, polarized_palette );
		
		p += 8 * n_colors;
	}
	
	bitmap.rowBytes = rowBytes;
	
	Rect srcRect = read_Rect( p );
	Rect dstRect = read_Rect( p );
	
	const short mode = read_word( p );
	
	if ( has_region )
	{
		short rgnSize = read_word( p );
		
		WARNING = "DrawPicture:  Ignoring bits region of size ", rgnSize;
		
		p += rgnSize - sizeof rgnSize;
	}
	
	short n_rows = bitmap.bounds.bottom - bitmap.bounds.top;
	
	if ( rowBytes < 8 )
	{
		bitmap.baseAddr = (Ptr) p;
		
		p += rowBytes * n_rows;
		
		StdBits( &bitmap, &srcRect, &dstRect, mode, NULL );
		
		return p;
	}
	
	bitmap.bounds.top    = 0;
	bitmap.bounds.bottom = 1;
	
	srcRect.top    = 0;
	srcRect.bottom = 1;
	
	dstRect.bottom = dstRect.top;
	
	UInt8* buffer = (UInt8*) alloca( rowBytes );
	
	bitmap.baseAddr = (Ptr) buffer;
	
	const int increment = 1 + (rowBytes > 250);
	
	do
	{
		p += increment;  // skip count
		
		UInt8* dst = buffer;
		
		quickdraw::unpack_bits( p, dst, rowBytes );
		
		if ( color  &&  pixmap.pixelSize == 8 )
		{
			apply_palette( bitmap, polarized_palette );
			
			downsample_8_to_1( buffer, buffer, rowBytes );
		}
		
		++dstRect.bottom;
		
		StdBits( &bitmap, &srcRect, &dstRect, mode, NULL );
		
		++dstRect.top;
	}
	while ( --n_rows > 0 );
	
	return p;
}
