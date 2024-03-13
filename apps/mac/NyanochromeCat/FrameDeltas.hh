/*
	FrameDeltas.hh
	--------------
*/

// Standard C
#include <stdint.h>

typedef unsigned char Byte;

void memxor_long( long* dst, const long* src, unsigned n );

inline
void memxor_aligned_32b( Byte* dst, const Byte* src, unsigned n )
{
	memxor_long( (long*) dst, (const long*) src, n / sizeof (long) );
}

void save_frame_delta( int i, Byte* dst, const Byte* src );
void load_frame_delta( int i, Byte* dst,       Byte* tmp );
