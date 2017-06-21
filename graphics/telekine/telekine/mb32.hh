/*
	mb32.hh
	-------
*/

#ifndef TELEKINE_MB32_HH
#define TELEKINE_MB32_HH

// iota
#include "iota/endian.hh"


namespace telekine
{
	
	inline
	uint32_t mb32_native_endian( char a, char b, char c, char d )
	{
		return (a << 24) | (b << 16) | (c << 8) | d;
	}
	
	inline
	uint32_t mb32( char a, char b, char c, char d )
	{
		return iota::is_little_endian() ? mb32_native_endian( d, c, b, a )
		                                : mb32_native_endian( a, b, c, d );
	}
	
}

#endif
