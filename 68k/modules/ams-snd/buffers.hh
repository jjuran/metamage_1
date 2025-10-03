/*
	buffers.hh
	----------
*/

#ifndef BUFFERS_HH
#define BUFFERS_HH

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// SoundDriver
#include "SoundDriver/SoundDriver.h"


struct SndChannel;

struct audio_buffer
{
	audio_buffer*  next;
	SndChannel*    ch;
	ParamBlockRec  pb;
	FFSynthRec     ff;
};

extern audio_buffer* free_audio_buffer;

audio_buffer* alloc_buffer();

void return_buffer( audio_buffer* buffer );

#endif
