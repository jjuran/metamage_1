/*
	bits2pict-test.cc
	-----------------
*/

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// POSIX
#include <unistd.h>

// Standard C
#include <stdlib.h>
#include <string.h>

// gear
#include "gear/hexadecimal.hh"

// bits
#include "bits/bits.hh"

// mac-glue-utils
#include "mac_glue/Memory.hh"

// mac-qd-utils
#include "mac_qd/scoped_temp_port.hh"


#pragma exceptions off


typedef unsigned char Byte;

#ifdef __MC68K__

static inline
asm
long has_A5_world()
{
	MOVE.L   (A5),D0
}

static inline
bool needs_InitGraf()
{
	return ! has_A5_world();
}

#else

static inline
bool needs_InitGraf()
{
	return false;
}

#endif

static inline
void CopyBits_in_place( const BitMap& bits )
{
	/*
		We're targeting the current graphics port anyway,
		so just use StdBits() instead of CopyBits().
	*/
	
	StdBits( &bits, &bits.bounds, &bits.bounds, srcCopy, NULL );
}

static inline
PicHandle picture_from_BitMap( const BitMap& bits )
{
	mac::qd::scoped_temp_port_basic port;
	
	/*
		In Advanced Mac Substitute, we're currently using
		the port's portRect for the picture frame.  Until
		we fix that, we'll set the portRect explicitly.
		
		(The topLeft of the bitmap bounds is always 0,0.)
	*/
	
	PortSize( bits.bounds.right, bits.bounds.bottom );
	
	SetPortBits( &bits );
	
	ClipRect( &bits.bounds );
	
	PicHandle pic = OpenPicture( &bits.bounds );
	
	CopyBits_in_place( bits );
	
	ClosePicture();
	
	return pic;
}

static
void test_bits2pict( char* hex, size_t n_hex )
{
	if ( n_hex < sizeof (bits::header) * 2 )
	{
		write( STDOUT_FILENO, "\n", 1 );
		
		return;
	}
	
	size_t n_packed = n_hex / 2;
	
	Byte* packed = (Byte*) hex + ((long) hex & 1);  // ensure 16-bit alignment
	
	for ( int i = 0;  i < n_packed;  ++i )
	{
		packed[ i ] = gear::decode_8_bit_hex( hex );
		
		hex += 2;
	}
	
	const bits::header& bits = *(const bits::header*) packed;
	
	if ( ! valid( bits, n_packed ) )
	{
		write( STDOUT_FILENO, "\n", 1 );
		
		return;
	}
	
	const int stride = bits.stride;
	const int height = bits.height;
	const int width  = bits.width;
	
	Ptr p = (Ptr) packed + sizeof (::bits::header);
	
	BitMap bitmap =
	{
		p,
		stride,
		{ 0, 0, height, width },
	};
	
	if ( PicHandle pic = picture_from_BitMap( bitmap ) )
	{
		Size size = mac::glue::GetHandleSize_raw( (Handle) pic );
		
		size_t n_hex = size * 2 + 1;
		
		if ( Ptr hex = NewPtr( n_hex ) )
		{
			Ptr hex_end = gear::hexpcpy_lower( hex, *pic, size );
			
			*hex_end = '\n';
			
			write( STDOUT_FILENO, hex, n_hex );
			
			DisposePtr( hex );
		}
		
		KillPicture( pic );
	}
}

int main( int argc, char** argv )
{
#if CALL_NOT_IN_CARBON
	
	QDGlobals qd;
	
	if ( needs_InitGraf() )
	{
		InitGraf( &qd.thePort );
	}
	
#endif
	
	char** args = argv + 1;
	
	while ( char* arg = *args++ )
	{
		test_bits2pict( arg, strlen( arg ) );
	}
	
	return 0;
}
