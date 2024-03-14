/*
	transcode/8x_1bpp_to_8bpp.cc
	----------------------------
*/

#include "transcode/8x_1bpp_to_8bpp.hh"


static inline
Byte* transcode_8x_1bpp_to_8bpp( Byte* p, Byte octet )
{
	typedef signed char int8_t;
	
	*p++ = (int8_t) octet < 0 ? 0x00 : 0xFF;  octet <<= 1;
	*p++ = (int8_t) octet < 0 ? 0x00 : 0xFF;  octet <<= 1;
	*p++ = (int8_t) octet < 0 ? 0x00 : 0xFF;  octet <<= 1;
	*p++ = (int8_t) octet < 0 ? 0x00 : 0xFF;  octet <<= 1;
	*p++ = (int8_t) octet < 0 ? 0x00 : 0xFF;  octet <<= 1;
	*p++ = (int8_t) octet < 0 ? 0x00 : 0xFF;  octet <<= 1;
	*p++ = (int8_t) octet < 0 ? 0x00 : 0xFF;  octet <<= 1;
	*p++ = (int8_t) octet < 0 ? 0x00 : 0xFF;
	
	return p;
}

void transcode_8x_1bpp_to_8bpp( const Byte* src, Byte* dst, int n )
{
	while ( n-- > 0 )
	{
		dst = transcode_8x_1bpp_to_8bpp( dst, *src++ );
	}
}
