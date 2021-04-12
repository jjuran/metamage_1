/*
	transcode/8x_1bpp_to_8bpp.hh
	----------------------------
*/

#ifndef TRANSCODE_8X1BPPTO8BPP_HH
#define TRANSCODE_8X1BPPTO8BPP_HH

// Standard C
#include <stdint.h>


void transcode_8x_1bpp_to_8bpp( const uint8_t* src, uint8_t* dst, int n );

inline
void transcode_8x_1bpp_to_8bpp( const void* src, void* dst, int n )
{
	transcode_8x_1bpp_to_8bpp( (const uint8_t*) src, (uint8_t*) dst, n );
}

#endif
