/*
	square-wave.hh
	--------------
*/

#ifndef MACSNDDRVRSYNTH_SQUAREWAVE_HH
#define MACSNDDRVRSYNTH_SQUAREWAVE_HH

#include "command-queue.hh"


size_t synthesize_square_wave_samples( sw_buffer& buffer, uint8_t* output, size_t size );

#endif
