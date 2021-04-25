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
