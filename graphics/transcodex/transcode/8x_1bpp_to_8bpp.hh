/*
	transcode/8x_1bpp_to_8bpp.hh
	----------------------------
*/

#ifndef TRANSCODE_8X1BPPTO8BPP_HH
#define TRANSCODE_8X1BPPTO8BPP_HH


typedef unsigned char Byte;


void transcode_8x_1bpp_to_8bpp( const Byte* src, Byte* dst, int n );

inline
void transcode_8x_1bpp_to_8bpp( const void* src, void* dst, int n )
{
	transcode_8x_1bpp_to_8bpp( (const Byte*) src, (Byte*) dst, n );
}

#endif
