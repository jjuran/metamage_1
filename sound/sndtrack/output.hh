/*
	output.hh
	---------
*/

#ifndef OUTPUT_HH
#define OUTPUT_HH

typedef short output_sample_t;

const int samples_per_buffer = 370;

struct sample_buffer
{
	output_sample_t data[ samples_per_buffer ];
};

#endif
