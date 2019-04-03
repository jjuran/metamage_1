/*
	square-wave.hh
	--------------
*/

#ifndef SYNTH_SQUAREWAVE_HH
#define SYNTH_SQUAREWAVE_HH

struct sample_buffer;

typedef struct SWSynthRec sw_buffer;

short sw_synth( sample_buffer& output, sw_buffer& rec, bool reset );

#endif
