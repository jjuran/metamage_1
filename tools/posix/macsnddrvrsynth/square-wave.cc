/*
	square-wave.cc
	--------------
*/

// Standard C++
#include <algorithm>
#include <inttypes.h>

// macsnddrvrsynth
#include "square-wave.hh"


size_t synthesize_square_wave_samples( sw_buffer& buffer, uint8_t* output, size_t size )
{
	size_t samples_synthesized = 0;
	
	const Tone* tone = buffer.current_tone;
	while ( tone->count != 0 && tone->amplitude != 0 && tone->duration != 0 )
	{
		const long long ns_per_sec = 1000 * 1000 * 1000;
		/*
			Use 40.24 fixed-point math.
			Divide `ns_per_sec` and the ( 44930ns * 783360 ) divisor by 100 to
			fit the maximum value of `period_samples` into 40.24 bits.
		*/
		uint64_t period_samples = ( ns_per_sec / 100 * tone->count << 24 ) / ( 4493ull * 78336 );
		uint64_t target_index = (uint64_t) buffer.elapsed_tone_samples << 24;
		
		// Start by taking the wave low. Real Macs appear to do this.
		unsigned short sample = tone->amplitude * ( target_index / period_samples & 1 );
		target_index += period_samples;
		
		ssize_t tone_samples_left = tone->duration * 370 - buffer.elapsed_tone_samples;
		while ( tone_samples_left > 0 && samples_synthesized < size )
		{
			size_t tone_sample_count = std::min( (size_t) tone_samples_left, (size_t) ( target_index >> 24 ) - buffer.elapsed_tone_samples );
			size_t sample_count = std::min( size - samples_synthesized, tone_sample_count );
			memset( output + samples_synthesized, sample, sample_count );
			buffer.elapsed_tone_samples += sample_count;
			
			/*
				Alternate between the full amplitude and zero.
				Mini vMac appears to do this. On a real Mac, AC coupling
				electrically removes this inherent bias from the output signal.
			*/
			sample ^= tone->amplitude;
			target_index += period_samples;
			
			tone_samples_left -= sample_count;
			samples_synthesized += sample_count;
		}
		
		if ( tone_samples_left == 0 )
		{
			++tone;
			buffer.elapsed_tone_samples = 0;
		}
		
		if ( samples_synthesized == size )
		{
			buffer.current_tone = tone;
			break;
		}
	}
	
	return samples_synthesized;
}
