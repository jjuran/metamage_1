/*
	clut_detail.hh
	--------------
*/

#ifndef RASTER_CLUTDETAIL_HH
#define RASTER_CLUTDETAIL_HH

// Standard C
#include <stddef.h>
#include <stdint.h>


namespace raster
{
	
	struct color
	{
		uint16_t value;
		uint16_t red;
		uint16_t green;
		uint16_t blue;
	};
	
	struct clut_header
	{
		uint32_t seed;
		uint16_t flags;
		uint8_t  reserved;
		uint8_t  max;  // 1 .. 255:  number of color entries - 1
	};
	
	struct clut_data : clut_header
	{
		color palette[ 256 ];
	};
	
	inline
	size_t sizeof_clut( const clut_header& clut )
	{
		return sizeof clut + (clut.max + 1) * sizeof (color);
	}
	
}

#endif
