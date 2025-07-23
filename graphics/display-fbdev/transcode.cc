/*
	transcode.cc
	------------
*/

#include "transcode.hh"

// Standard C
#include <string.h>


uint8_t monochrome_toggle;

uint32_t palette_entries[ 256 ];

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
	const uint32_t* q = (const uint32_t*) src;
	
	uint32_t* p = (uint32_t*) dst;
	
	while ( width > 0 )
	{
		// Convert RGBA to ARGB.
		
		const uint32_t rgba = *q++;
		
		const uint32_t argb = rgba << 24
		                    | rgba >>  8;
		
		*p++ = argb;
		
		--width;
	}
}

void rgba_32_2x( const uint8_t* src, uint8_t* dst, int width )
{
	const uint32_t* q = (const uint32_t*) src;
	
	uint32_t* p = (uint32_t*) dst;
	
	while ( width-- > 0 )
	{
		// Convert RGBA to ARGB.
		
		const uint32_t rgba = *q++;
		
		const uint32_t argb = rgba << 24
		                    | rgba >>  8;
		
		*p++ = argb;
		*p++ = argb;
	}
}

void both_32( const uint8_t* src, uint8_t* dst, int width )
{
	const uint32_t* q = (const uint32_t*) src;
	
	uint32_t* p = (uint32_t*) dst;
	
	while ( width > 0 )
	{
		// Convert byte-swapped RGBA (ABGR) to ARGB.
		
		const uint32_t abgr = *q++;
		
		const uint32_t argb = (abgr & 0xFF00FF00)
		                    | (abgr & 0x00FF00FF) >> 16
		                    | (abgr & 0x00FF00FF) << 16;
		
		*p++ = argb;
		
		--width;
	}
}

void both_32_2x( const uint8_t* src, uint8_t* dst, int width )
{
	const uint32_t* q = (const uint32_t*) src;
	
	uint32_t* p = (uint32_t*) dst;
	
	while ( width-- > 0 )
	{
		// Convert byte-swapped RGBA (ABGR) to ARGB.
		
		const uint32_t abgr = *q++;
		
		const uint32_t argb = (abgr & 0xFF00FF00)
		                    | (abgr & 0x00FF00FF) >> 16
		                    | (abgr & 0x00FF00FF) << 16;
		
		*p++ = argb;
		*p++ = argb;
	}
}

void rgb555_BE_to_565_LE( const uint8_t* src, uint8_t* dst, int width )
{
	uint16_t* p = (uint16_t*) dst;
	
	while ( width-- > 0 )
	{
		const uint8_t msb = *src++;
		const uint8_t lsb = *src++;
		
		const uint16_t xrgb = msb << 8 | lsb;
		
		const uint16_t r = (xrgb & 0x7C00) << 1;
		const uint16_t g = (xrgb & 0x03E0) << 1 | (xrgb & 0x0200) >> 4;
		const uint16_t b = (xrgb & 0x001F);
		
		const uint16_t rgb = r | g | b;
		
		*p++ = rgb;
	}
}

void rgb555_BE_to_565_LE_2x( const uint8_t* src, uint8_t* dst, int width )
{
	uint16_t* p = (uint16_t*) dst;
	
	while ( width-- > 0 )
	{
		const uint8_t msb = *src++;
		const uint8_t lsb = *src++;
		
		const uint16_t xrgb = msb << 8 | lsb;
		
		const uint16_t r = (xrgb & 0x7C00) << 1;
		const uint16_t g = (xrgb & 0x03E0) << 1 | (xrgb & 0x0200) >> 4;
		const uint16_t b = (xrgb & 0x001F);
		
		const uint16_t rgb = r | g | b;
		
		*p++ = rgb;
		*p++ = rgb;
	}
}

void rgb555_BE_to_32( const uint8_t* src, uint8_t* dst, int width )
{
	uint32_t* p = (uint32_t*) dst;
	
	while ( width > 0 )
	{
		const uint8_t msb = *src++;
		const uint8_t lsb = *src++;
		
		const uint16_t rgb = msb << 8 | lsb;
		
		const uint8_t r = (rgb & 0x7C00) * 0x0021 >> (10 + 2);
		const uint8_t g = (rgb & 0x03E0) * 0x0021 >> ( 5 + 2);
		const uint8_t b = (rgb & 0x001F) * 0x0021 >> ( 0 + 2);
		
		const uint32_t xrgb = r << 16
		                    | g <<  8
		                    | b;
		
		*p++ = xrgb;
		
		--width;
	}
}

void rgb555_BE_to_32_2x( const uint8_t* src, uint8_t* dst, int width )
{
	uint32_t* p = (uint32_t*) dst;
	
	while ( width-- > 0 )
	{
		const uint8_t msb = *src++;
		const uint8_t lsb = *src++;
		
		const uint16_t rgb = msb << 8 | lsb;
		
		const uint8_t r = (rgb & 0x7C00) * 0x0021 >> (10 + 2);
		const uint8_t g = (rgb & 0x03E0) * 0x0021 >> ( 5 + 2);
		const uint8_t b = (rgb & 0x001F) * 0x0021 >> ( 0 + 2);
		
		const uint32_t xrgb = r << 16
		                    | g <<  8
		                    | b;
		
		*p++ = xrgb;
		*p++ = xrgb;
	}
}

void rgb565_LE_to_32( const uint8_t* src, uint8_t* dst, int width )
{
	uint32_t* p = (uint32_t*) dst;
	
	while ( width > 0 )
	{
		const uint8_t lsb = *src++;
		const uint8_t msb = *src++;
		
		const uint16_t rgb = msb << 8 | lsb;
		
		const uint8_t r = (rgb & 0xF800) * 0x0021 >> (11 + 2);
		const uint8_t g = (rgb & 0x07E0) * 0x0041 >> ( 5 + 4);
		const uint8_t b = (rgb & 0x001F) * 0x0021 >> ( 0 + 2);
		
		const uint32_t xrgb = r << 16
		                    | g <<  8
		                    | b;
		
		*p++ = xrgb;
		
		--width;
	}
}

void rgb565_LE_to_32_2x( const uint8_t* src, uint8_t* dst, int width )
{
	uint32_t* p = (uint32_t*) dst;
	
	while ( width-- > 0 )
	{
		const uint8_t lsb = *src++;
		const uint8_t msb = *src++;
		
		const uint16_t rgb = msb << 8 | lsb;
		
		const uint8_t r = (rgb & 0xF800) * 0x0021 >> (11 + 2);
		const uint8_t g = (rgb & 0x07E0) * 0x0041 >> ( 5 + 4);
		const uint8_t b = (rgb & 0x001F) * 0x0021 >> ( 0 + 2);
		
		const uint32_t xrgb = r << 16
		                    | g <<  8
		                    | b;
		
		*p++ = xrgb;
		*p++ = xrgb;
	}
}
