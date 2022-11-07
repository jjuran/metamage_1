/*
	palette.cc
	----------
*/

#include "palette.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// ams-common
#include "math.hh"


void polarize_palette( const ColorSpec* palette, short size, Byte* polarized )
{
	for ( int i = 0;  i < size;  ++i )
	{
		const RGBColor& color = palette[ i ].rgb;
		
		/*
			Since an individual color channel ranges from 0 to 0xFFFF,
			the maximum intensity is 7 * 0xFFFF (or 7x).
			
			The eight color cube vertices:
			
				black:   0x -> black
				blue:    1x -> black
				red:     2x -> black
				magenta: 3x -> black
				green:   4x -> black
				cyan:    5x -> white
				yellow:  6x -> white
				white:   7x -> white
				
			Grayscale:
			
				$0000 -> black
				$1111 -> black
				$2222 -> black
				$3333 -> black
				$4444 -> black
				$5555 -> black
				$6666 -> black
				$7777 -> black
				$8888 -> black
				$9999 -> white
				$AAAA -> white
				$BBBB -> white
				$CCCC -> white
				$DDDD -> white
				$EEEE -> white
				$FFFF -> white
			
			The multipliers and the threshold may need some tuning.
		*/
		
		UInt32 intensity = color.red   * 2
		                 + color.green * 4
		                 + color.blue;
		
		polarized[ i ] = intensity > 4 * 0xFFFF ? 0x00 : 0xFF;
	}
}

void apply_palette( const BitMap& bits, const Byte* polarized_palette )
{
	Ptr p = bits.baseAddr;
	
	const UInt16 rowBytes = bits.rowBytes;
	
	const UInt16 height = bits.bounds.bottom - bits.bounds.top;
	
	UInt32 n_pixels = mulu_w( height, rowBytes );
	
	do
	{
		Byte index = *p;
		
		*p++ = polarized_palette[ index ];
	}
	while ( --n_pixels > 0 );
}
