/*
	spec.hh
	-------
*/

#ifndef RASTER_SPEC_HH
#define RASTER_SPEC_HH

// Standard C
#include <stdint.h>


namespace raster
{
	
	struct raster_spec
	{
		uint32_t  width;
		uint32_t  height;
	};
	
	long parse_raster_spec( raster_spec& spec, const char* text );
	
}

#endif
