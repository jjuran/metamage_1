/*
	transcode/8x_1bpp_to_8bpp.cc
	----------------------------
*/

#include "transcode/8x_1bpp_to_8bpp.hh"


namespace transcode
{

static inline
Byte* _8x_1bpp_to_8bpp( Byte* p, Byte octet, Byte z, Byte o )
{
	typedef signed char int8_t;
	
	*p++ = (int8_t) octet < 0 ? o : z;  octet <<= 1;
	*p++ = (int8_t) octet < 0 ? o : z;  octet <<= 1;
	*p++ = (int8_t) octet < 0 ? o : z;  octet <<= 1;
	*p++ = (int8_t) octet < 0 ? o : z;  octet <<= 1;
	*p++ = (int8_t) octet < 0 ? o : z;  octet <<= 1;
	*p++ = (int8_t) octet < 0 ? o : z;  octet <<= 1;
	*p++ = (int8_t) octet < 0 ? o : z;  octet <<= 1;
	*p++ = (int8_t) octet < 0 ? o : z;
	
	return p;
}

void _8x_1bpp_to_8bpp( const Byte* src, Byte* dst, int n, Byte z, Byte o )
{
	while ( n-- > 0 )
	{
		dst = _8x_1bpp_to_8bpp( dst, *src++, z, o );
	}
}

}
