/*
	screen.cc
	---------
*/

#include "raster/screen.hh"

// raster
#include "raster/raster.hh"


namespace raster
{
	
	raster_load mac_screen_raster_load( void* addr )
	{
		const uint32_t SCRN = kStartupScreenFileType;
		
		static raster_metadata meta =
		{{
			SCRN, // magic
			0,    // version
			512,  // width
			342,  // height
			64,   // stride (a.k.a. rowBytes)
			1,    // weight
			Model_monochrome_paint,
		}};
		
		raster_load load = { addr, mac_screen_size, &meta };
		
		return load;
	}
	
}
