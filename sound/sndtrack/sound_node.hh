/*
	sound_node.hh
	-------------
*/

#ifndef SOUNDNODE_HH
#define SOUNDNODE_HH

// sndpipe-api
#include "sndpipe/sndpipe.hh"

// sndtrack
#include "queues.hh"


typedef SWSynthRec sw_buffer;
typedef FFSynthRec ff_buffer;

typedef sndpipe::FTSynthRec_flat_buffer ft_buffer;
typedef sndpipe::FTSynthRec_wave_update wu_buffer;

union sound_buffer
{
	short mode;
	sw_buffer square_wave;
	ft_buffer four_tone;
	wu_buffer wave_data;
	ff_buffer free_form;
};

struct sound_node : queue_node
{
	sound_buffer sound;
};

#endif
