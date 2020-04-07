/*
	free-form.cc
	------------
*/

// Standard C++
#include <algorithm>

// macsnddrvrsynth
#include "free-form.hh"


size_t synthesize_free_form_samples( ff_buffer& buffer, uint8_t* output, size_t size )
{
	size_t samples_synthesized = 0;
	
	Fixed sample_index = buffer.rec.count * buffer.elapsed_samples;
	size_t byte_index = sample_index >> 16;
	while ( byte_index < buffer.size && samples_synthesized < size )
	{
		output[ samples_synthesized ] = buffer.rec.waveBytes[ sample_index >> 16 ];
		sample_index += buffer.rec.count;
		byte_index = sample_index >> 16;
		++samples_synthesized;
	}
	buffer.elapsed_samples += samples_synthesized;
	
	return samples_synthesized;
}
