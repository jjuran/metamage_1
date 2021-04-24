/*
	transcode/8x_1bpp_to_8bpp.cc
	----------------------------
*/

#include "transcode/8x_1bpp_to_8bpp.hh"


static inline
uint8_t* transcode_8x_1bpp_to_8bpp( uint8_t* p, uint8_t octet )
{
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

void transcode_8x_1bpp_to_8bpp( const uint8_t* src, uint8_t* dst, int n )
{
	while ( n-- > 0 )
	{
		dst = transcode_8x_1bpp_to_8bpp( dst, *src++ );
	}
}
