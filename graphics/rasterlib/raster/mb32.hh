/*
	mb32.hh
	-------
*/

#ifndef RASTER_MB32_HH
#define RASTER_MB32_HH

// iota
#include "iota/endian.hh"


namespace raster
{
	
	typedef unsigned char uint8_t;
	
	inline
	uint32_t mb32_native_endian( uint8_t a, uint8_t b, uint8_t c, uint8_t d )
	{
		return (a << 24) | (b << 16) | (c << 8) | d;
	}
	
	inline
	uint32_t mb32( uint8_t a, uint8_t b, uint8_t c, uint8_t d )
	{
		return iota::is_little_endian() ? mb32_native_endian( d, c, b, a )
		                                : mb32_native_endian( a, b, c, d );
	}
	
}

#endif
