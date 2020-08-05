/*
	free-form.cc
	------------
*/

#include "synth/free-form.hh"

// Standard C
#include <stdint.h>

// SoundDriver
#include "SoundDriver/SoundDriver.h"

// sndtrack
#include "output.hh"


static uint32_t elapsed_samples;

short ff_synth( sample_buffer& output, int size, ff_buffer& rec, bool reset )
{
	if ( reset )
	{
		elapsed_samples = 0;
	}
	
	output_sample_t* p = output.data;
	
	uint32_t samples_remaining = samples_per_buffer;
	
	uint32_t wave_point = rec.count * elapsed_samples;
	uint16_t wave_index = wave_point >> 16;
	
	if ( wave_index >= size )
	{
		return -1;
	}
	
	do
	{
		*p++ = rec.waveBytes[ wave_index ] ^ 0x80;
		
		++elapsed_samples;
		--samples_remaining;
		
		if ( samples_remaining == 0 )
		{
			return samples_per_buffer;
		}
		
		wave_point += rec.count;
		wave_index = wave_point >> 16;
	}
	while ( wave_index < size );
	
	return samples_per_buffer - samples_remaining;
}
