/*
	sound_node.hh
	-------------
*/

#ifndef SOUNDNODE_HH
#define SOUNDNODE_HH

// exosnd-api
#include "exosnd/exosnd.hh"

// sndtrack
#include "queues.hh"


typedef SWSynthRec sw_buffer;
typedef FFSynthRec ff_buffer;

typedef exosnd::FTSynthRec_flat_buffer ft_buffer;

union sound_buffer
{
	short mode;
	sw_buffer square_wave;
	ft_buffer four_tone;
	ff_buffer free_form;
};

struct sound_node : queue_node
{
	sound_buffer sound;
};

#endif
