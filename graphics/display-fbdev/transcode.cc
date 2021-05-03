/*
	transcode.cc
	------------
*/

#include "transcode.hh"

// Standard C
#include <string.h>


const uint8_t monochrome_1bit[] =
{
	0x00,
	0xFF,
};

const uint8_t monochrome_2bit[] =
{
	0x00, 0x55,
	0xAA, 0xFF,
};

const uint8_t monochrome_4bit[] =
{
	0x00, 0x11, 0x22, 0x33,
	0x44, 0x55, 0x66, 0x77,
	0x88, 0x99, 0xAA, 0xBB,
	0xCC, 0xDD, 0xEE, 0xFF,
};

void copy_16( const uint8_t* src, uint8_t* dst, int width )
{
	memcpy( dst, src, width * 2 );
}

void copy_16_2x( const uint8_t* src, uint8_t* dst, int width )
{
	uint16_t* p = (uint16_t*) dst;
	uint16_t* q = (uint16_t*) src;
	
	while ( width-- > 0 )
	{
		uint16_t pixel = *q++;
		
		*p++ = pixel;
		*p++ = pixel;
	}
}

void copy_32( const uint8_t* src, uint8_t* dst, int width )
{
	memcpy( dst, src, width * 4 );
}

void copy_32_2x( const uint8_t* src, uint8_t* dst, int width )
{
	uint32_t* p = (uint32_t*) dst;
	uint32_t* q = (uint32_t*) src;
	
	while ( width-- > 0 )
	{
		uint32_t pixel = *q++;
		
		*p++ = pixel;
		*p++ = pixel;
	}
}

void swap_32( const uint8_t* src, uint8_t* dst, int width )
{
	while ( width > 0 )
	{
		uint8_t a = *src++;
		uint8_t b = *src++;
		uint8_t c = *src++;
		uint8_t d = *src++;
		
		*dst++ = d;
		*dst++ = c;
		*dst++ = b;
		*dst++ = a;
		
		--width;
	}
}

void swap_32_2x( const uint8_t* src, uint8_t* dst, int width )
{
	while ( width-- > 0 )
	{
		uint8_t a = *src++;
		uint8_t b = *src++;
		uint8_t c = *src++;
		uint8_t d = *src++;
		
		*dst++ = d;
		*dst++ = c;
		*dst++ = b;
		*dst++ = a;
		
		*dst++ = d;
		*dst++ = c;
		*dst++ = b;
		*dst++ = a;
	}
}

void rgba_32( const uint8_t* src, uint8_t* dst, int width )
{
	uint32_t* p = (uint32_t*) dst;
	
	while ( width > 0 )
	{
		// We're assuming that RGBA is big-endian, as in Android 4.
		
		uint8_t r = *src++;
		uint8_t g = *src++;
		uint8_t b = *src++;
		uint8_t a = *src++;
		
		const uint32_t argb = a << 24
		                    | r << 16
		                    | g <<  8
		                    | b;
		
		*p++ = argb;
		
		--width;
	}
}

void rgba_32_2x( const uint8_t* src, uint8_t* dst, int width )
{
	uint32_t* p = (uint32_t*) dst;
	
	while ( width-- > 0 )
	{
		// We're assuming that RGBA is big-endian, as in Android 4.
		
		uint8_t r = *src++;
		uint8_t g = *src++;
		uint8_t b = *src++;
		uint8_t a = *src++;
		
		const uint32_t argb = a << 24
		                    | r << 16
		                    | g <<  8
		                    | b;
		
		*p++ = argb;
		*p++ = argb;
	}
}

void rgb565_16( const uint8_t* src, uint8_t* dst, int width )
{
	uint32_t* p = (uint32_t*) dst;
	
	while ( width > 0 )
	{
		// We're assuming that RGB 5/6/5 is little-endian.
		
		const uint8_t lsb = *src++;
		const uint8_t msb = *src++;
		
		const uint16_t rgb = msb << 8 | lsb;
		
		const uint8_t r = (rgb & 0xF800) * 0x0021 >> 11 + 2;
		const uint8_t g = (rgb & 0x07E0) * 0x0041 >>  5 + 4;
		const uint8_t b = (rgb & 0x001F) * 0x0021 >>  0 + 2;
		
		const uint32_t xrgb = r << 16
		                    | g <<  8
		                    | b;
		
		*p++ = xrgb;
		
		--width;
	}
}

void rgb565_16_2x( const uint8_t* src, uint8_t* dst, int width )
{
	uint32_t* p = (uint32_t*) dst;
	
	while ( width-- > 0 )
	{
		// We're assuming that RGB 5/6/5 is little-endian.
		
		const uint8_t lsb = *src++;
		const uint8_t msb = *src++;
		
		const uint16_t rgb = msb << 8 | lsb;
		
		const uint8_t r = (rgb & 0xF800) * 0x0021 >> 11 + 2;
		const uint8_t g = (rgb & 0x07E0) * 0x0041 >>  5 + 4;
		const uint8_t b = (rgb & 0x001F) * 0x0021 >>  0 + 2;
		
		const uint32_t xrgb = r << 16
		                    | g <<  8
		                    | b;
		
		*p++ = xrgb;
		*p++ = xrgb;
	}
}
