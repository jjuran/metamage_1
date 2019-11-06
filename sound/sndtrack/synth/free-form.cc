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
	
	uint8_t* p = output.data;
	
	uint32_t samples_remaining = sizeof output.data;
	
	uint32_t wave_point = rec.count * elapsed_samples;
	uint16_t wave_index = wave_point >> 16;
	
	if ( wave_index >= size )
	{
		return -1;
	}
	
	do
	{
		*p++ = rec.waveBytes[ wave_index ];
		
		++elapsed_samples;
		--samples_remaining;
		
		if ( samples_remaining == 0 )
		{
			return sizeof output.data;
		}
		
		wave_point += rec.count;
		wave_index = wave_point >> 16;
	}
	while ( wave_index < size );
	
	return sizeof output.data - samples_remaining;
}
