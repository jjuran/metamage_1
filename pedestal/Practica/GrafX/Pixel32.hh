/*	================
 *	GrafX/Pixel32.hh
 *	================
 */

#ifndef GRAFX_PIXEL32_HH
#define GRAFX_PIXEL32_HH

// Mac OS
#include <Quickdraw.h>


namespace GrafX
{
	
	struct Pixel32
	{
		unsigned char alpha;
		unsigned char red;
		unsigned char green;
		unsigned char blue;
		
		Pixel32()  {}
		
		Pixel32( const ::RGBColor& color );
	};
	
	inline Pixel32::Pixel32( const ::RGBColor& color ) : alpha( 0                ),
	                                                     red  ( color.red   >> 8 ),
	                                                     green( color.green >> 8 ),
	                                                     blue ( color.blue  >> 8 )
	{
	}
	
	
	struct Pixel32_Direct
	{
	#if TARGET_RT_BIG_ENDIAN
		
		unsigned char alpha;
		unsigned char red;
		unsigned char green;
		unsigned char blue;
		
	#else
		
		unsigned char blue;
		unsigned char green;
		unsigned char red;
		unsigned char alpha;
		
	#endif
		
		Pixel32_Direct()  {}
		
		Pixel32_Direct( const ::RGBColor& color );
	};
	
	inline Pixel32_Direct::Pixel32_Direct( const ::RGBColor& color )
	{
		alpha = 0;
		
		red   = color.red   >> 8;
		green = color.green >> 8;
		blue  = color.blue  >> 8;
	}
	
}

#endif

