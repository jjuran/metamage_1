/*
	colorspaces.cc
	--------------
*/

#include "mac_cg/colorspaces.hh"


namespace mac {
namespace cg  {

/*
	The CGColorSpace reference says:
	
		"In Mac OS X v10.4 and later, this color space
		is no longer device-dependent and is replaced
		by the generic counterpart"
	
	in its description of CGColorSpaceCreateDevice{Gray,RGB}().
*/

CGColorSpaceRef generic_or_device_gray()
{
	static CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceGray();
	
	return colorSpace;
}

CGColorSpaceRef generic_or_device_RGB()
{
	static CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
	
	return colorSpace;
}

CGColorSpaceRef create_inverted_grayscale( int count )
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
	
	CGColorSpaceRef gray = generic_or_device_gray();
	
	return CGColorSpaceCreateIndexed( gray, count - 1, table );
}

CGColorSpaceRef create_RGB_palette( const UInt16*  colors,
                                    int            count )
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
	
	CGColorSpaceRef rgb = generic_or_device_RGB();
	
	return CGColorSpaceCreateIndexed( rgb, count - 1, table );
	
}

}
}
