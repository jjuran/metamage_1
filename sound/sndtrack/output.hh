/*
	output.hh
	---------
*/

#ifndef OUTPUT_HH
#define OUTPUT_HH

typedef signed char output_sample_t;

const int samples_per_buffer = 370;

struct sample_buffer
{
	output_sample_t data[ samples_per_buffer ];
};

#endif
