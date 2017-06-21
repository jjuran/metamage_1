/*
	types.hh
	--------
*/

#ifndef TELEKINE_TYPES_HH
#define TELEKINE_TYPES_HH

// Standard C
#include <stdint.h>

// config
#include "config/endian.h"


namespace telekine
{
	
	struct command_header
	{
		uint32_t  command;
		uint32_t  size;  // size of payload, exclusing the header
	};
	
	struct raster_desc
	{
		// meta
		uint32_t  magic;
		uint32_t  version;
		
		// dimensions
		uint32_t  width;   // pixels per row (not including gutter)
		uint32_t  height;  // rows per frame
		
		uint32_t  stride;  // bytes per row
		uint8_t   weight;  // bits per pixel
		uint8_t   model;   // raster_model
		uint16_t  flags;
	};
	
	struct rect_pv16  // point + vector, 16-bit components
	{
		uint16_t  left;
		uint16_t  top;
		uint16_t  width;
		uint16_t  height;
	};
	
	void swap_bytes( command_header& header );
	
	void swap_bytes( raster_desc& desc );
	
	void swap_bytes( rect_pv16& rect );
	
	template < class Structure >
	inline
	void convert_big_endian( Structure& structure )
	{
		if ( CONFIG_LITTLE_ENDIAN )
		{
			swap_bytes( structure );
		}
	}
	
}

#endif
