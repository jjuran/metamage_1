/*
	colorspaces.cc
	--------------
*/

#include "mac_cg/colorspaces.hh"


#ifndef CONFIG_GENERIC_COLORSPACES
#ifdef MAC_OS_X_VERSION_10_4
#define CONFIG_GENERIC_COLORSPACES  1
#else
#define CONFIG_GENERIC_COLORSPACES  0
#endif
#endif

#if CONFIG_GENERIC_COLORSPACES
#define CREATE_GRAY_CS() CGColorSpaceCreateWithName( kCGColorSpaceGenericGray )
#define CREATE_RGB_CS()  CGColorSpaceCreateWithName( kCGColorSpaceGenericRGB  )
#else
#define CREATE_GRAY_CS() CGColorSpaceCreateDeviceGray()
#define CREATE_RGB_CS()  CGColorSpaceCreateDeviceRGB()
#endif


namespace mac {
namespace cg  {

CGColorSpaceRef generic_or_device_gray()
{
	static CGColorSpaceRef colorSpace = CREATE_GRAY_CS();
	
	return colorSpace;
}

CGColorSpaceRef generic_or_device_RGB()
{
	static CGColorSpaceRef colorSpace = CREATE_RGB_CS();
	
	return colorSpace;
}

CGColorSpaceRef create_inverted_grayscale( CGColorSpaceRef gray, int count )
{
	if ( (unsigned) count > 256 )
	{
		return NULL;
	}
	
	/*
		  2 -> 255 div 1   == 0xFF  // 255
		  4 -> 255 div 3   == 0x55  //  85
		 16 -> 255 div 15  == 0x11  //  17
		256 -> 255 div 255 == 0x01  //   1
	*/
	
	Byte delta = 0xFF / (count - 1);
	Byte value = 0xFF;
	
	Byte table[ 256 ];
	
	Byte* p = table;
	
	for ( int i = 0;  i < count;  ++i )
	{
		*p++ = value;
		
		value -= delta;
	}
	
	return CGColorSpaceCreateIndexed( gray, count - 1, table );
}

CGColorSpaceRef create_RGB_palette( CGColorSpaceRef  rgb,
                                    const UInt16*    colors,
                                    int              count )
{
	if ( (unsigned) count > 256 )
	{
		return NULL;
	}
	
	Byte table[ 3 * 256 ];
	
	Byte* p = table;
	
	for ( int i = 0;  i < count;  ++i )
	{
		++colors;  // skip non-channel value
		
		*p++ = *colors++ >> 8;  // red
		*p++ = *colors++ >> 8;  // green
		*p++ = *colors++ >> 8;  // blue
	}
	
	return CGColorSpaceCreateIndexed( rgb, count - 1, table );
	
}

}
}
