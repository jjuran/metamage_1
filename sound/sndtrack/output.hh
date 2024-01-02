/*
	output.hh
	---------
*/

#ifndef OUTPUT_HH
#define OUTPUT_HH

typedef short output_sample_t;

const int channel_count = 1;  // monophonic
const int sample_size   = sizeof (output_sample_t);
const int frame_size    = sample_size * channel_count;

const int frames_per_buffer  = 370;
const int samples_per_buffer = frames_per_buffer * channel_count;

struct sample_buffer
{
	short            count;
	output_sample_t  data[ samples_per_buffer ];
};

#endif
