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
	
	enum
	{
		Chroma_default,
		Chroma_grayscale,
		Chroma_color = -1,
	};
	
	enum
	{
		Endian_native,
		Endian_little,
		Endian_big = -1,
	};
	
	struct raster_spec
	{
		uint32_t  width;
		uint32_t  height;
		uint8_t   weight;
		int8_t    chroma;
		int8_t    endian;
	};
	
	inline
	bool is_color( const raster_spec& spec )
	{
		return spec.chroma < 0;
	}
	
	inline
	bool is_grayscale( const raster_spec& spec )
	{
		return spec.chroma > 0;
	}
	
	long parse_raster_spec( raster_spec& spec, const char* text );
	
}

#endif
