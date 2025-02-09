/*
	waveform.hh
	-----------
*/

#ifndef WAVEFORM_HH
#define WAVEFORM_HH

typedef unsigned char Byte;

extern Byte triangle_wave[ 256 ];
extern Byte harmonic_wave[ 256 ];

void set_waveform( const Byte* wave );

#endif
