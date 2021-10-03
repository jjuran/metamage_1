/*
	FrameDeltas.cc
	--------------
*/

#include "FrameDeltas.hh"

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __PROCESSES__
#include <Processes.h>
#endif

// damogran
#include "damogran/unpack.hh"
#include "damogran/pack.hh"

// nyancatlib
#include "nyancat/graphics.hh"

// NyanochromeCat
#include "Geometry.hh"


using nyancat::n_frames;


static Handle frame_deltas[ n_frames ];

static inline
Handle OK( Handle h )
{
	if ( ! h )
	{
		ExitToShell();
	}
	
	return h;
}

static
void memxor_long( long* dst, const long* src, unsigned n )
{
	while ( n-- > 0 )
	{
		*dst++ ^= *src++;
	}
}

static inline
void memxor_aligned_32b( uint8_t* dst, const uint8_t* src, unsigned n )
{
	memxor_long( (long*) dst, (const long*) src, n / sizeof (long) );
}

static
Handle NewHandle_with_packed_data( const uint8_t* data, Size unpacked_size )
{
	const uint8_t* end = data + unpacked_size;
	
	Size packed_size = damogran::pack_preflight( data, end );
	
	Handle h = NewHandle( packed_size );
	
	if ( h )
	{
		damogran::pack( data, end, (uint8_t*) *h );
	}
	
	return h;
}

void save_frame_delta( int i, uint8_t* dst, const uint8_t* src )
{
	memxor_aligned_32b( dst, src, frame_size );
	
	frame_deltas[ i ] = OK( NewHandle_with_packed_data( dst, frame_size ) );
}

void load_frame_delta( int i, uint8_t* dst, uint8_t* tmp )
{
	const uint8_t* src = (uint8_t*) *frame_deltas[ i ];
	
	damogran::unpack( src, tmp, tmp + frame_size );
	
	memxor_aligned_32b( dst, tmp, frame_size );
}
