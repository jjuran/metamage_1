/*
	waveform.cc
	-----------
*/

#include "waveform.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif


#pragma exceptions off


typedef signed char  SInt8;
typedef signed short SInt16;

Byte triangle_wave[ 256 ];
Byte harmonic_wave[ 256 ];

static Byte temp_wave_x2[ 256 ];
static Byte temp_wave_x3[ 256 ];
static Byte temp_wave_x4[ 256 ];


static inline
void my_memcpy( void* dst, const void* src, unsigned n )
{
	BlockMoveData( src, dst, n );
}

static
Byte fill_triangle( Byte* p )
{
	for ( int i = 64;  i < 128;  ++i )
	{
		*p++ = 1 + i * 2;
	}
	
	for ( int i = 127;  i >= 0;  --i )
	{
		*p++ = 1 + i * 2;
	}
	
	for ( int i = 0;  i < 64;  ++i )
	{
		*p++ = 1 + i * 2;
	}
	
	return 0;
}

static
void wavecopy_octave( Byte* dst, const Byte* src )
{
	for ( int i = 0;  i < 128;  ++i )
	{
		dst[ i ] = src[ i * 2 ];
	}
	
	my_memcpy( dst + 128, dst, 128 );
}

static
void wavecopy_oct_5th( Byte* dst, const Byte* src )
{
	for ( int i = 0;  i < 256;  ++i )
	{
		dst[ i ] = src[ i * 3 % 256 ];
	}
}

static
void blend( Byte* dst, const Byte* a, const Byte* b )
{
	for ( int i = 0;  i < 256;  ++i )
	{
		SInt16 sum = (SInt8) (a[ i ] - 128)
		           + (SInt8) (b[ i ] - 128);
		
		dst[ i ] = sum / 2 + 128;
	}
}

static
bool make_waves()
{
	fill_triangle( triangle_wave );
	set_waveform ( triangle_wave );
	
	return true;
}

void set_waveform( const Byte* wave )
{
	wavecopy_octave( temp_wave_x2, wave );
	wavecopy_octave( temp_wave_x4, temp_wave_x2 );
	
	wavecopy_oct_5th( temp_wave_x3, wave );
	
	Byte tmp[ 256 ];
	
	blend( tmp, temp_wave_x3, temp_wave_x4 );
	blend( tmp, temp_wave_x2, tmp );
	
	blend( harmonic_wave, wave, tmp );
}

static bool made_waves = make_waves();
