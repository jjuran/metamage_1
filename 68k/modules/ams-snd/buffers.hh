/*
	buffers.hh
	----------
*/

#ifndef BUFFERS_HH
#define BUFFERS_HH

// SoundDriver
#include "SoundDriver/SoundDriver.h"


struct SndChannel;

struct audio_buffer
{
	audio_buffer*  next;
	SndChannel*    ch;
	FFSynthRec     ff;
};

extern audio_buffer* free_audio_buffer;

audio_buffer* alloc_buffer();

void return_buffer( audio_buffer* buffer );

#endif
