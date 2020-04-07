/*
	command-queue.hh
	----------------
*/

#ifndef MACSNDDRVRSYNTH_COMMANDQUEUE_HH
#define MACSNDDRVRSYNTH_COMMANDQUEUE_HH

// SoundDriver
#include "SoundDriver/SoundDriver.h"


struct sw_buffer
{
	SWSynthRec rec;
	const Tone* current_tone;
	size_t elapsed_tone_samples;
};

struct ft_buffer
{
	FTSynthRec rec;
	// TODO
};

struct ff_buffer
{
	FFSynthRec rec;
	size_t size;
	size_t elapsed_samples;
};

union sound_buffer
{
	SInt16 mode;
	sw_buffer square_wave;
	ft_buffer four_tone;
	ff_buffer free_form;
};

int read_fd_ready( int fd );

void read_into_queue( int fd );

sound_buffer* peek_queue_front();

void pop_from_queue();

void clear_queue();

#endif
