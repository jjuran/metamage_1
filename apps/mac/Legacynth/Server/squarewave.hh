/*
	squarewave.hh
	-------------
*/

#ifndef SQUAREWAVE_HH
#define SQUAREWAVE_HH

typedef char* Ptr;

struct Tone;

long squarewave_fill( const Tone* tone, Ptr last, Ptr p, long n );

#endif
