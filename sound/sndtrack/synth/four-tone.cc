/*
	four-tone.cc
	------------
*/

#include "synth/four-tone.hh"

// Standard C
#include <stdint.h>

// exosnd-api
#include "exosnd/exosnd.hh"

// sndtrack
#include "output.hh"


// sample indices
static Fixed si1;
static Fixed si2;
static Fixed si3;
static Fixed si4;

static
int8_t sample( Wave wave, Fixed index )
{
	uint8_t i = index >> 16;
	uint8_t j = i + 1;
	
	int8_t a = wave[ i ] ^ 0x80;
	int8_t b = wave[ j ] ^ 0x80;
	
	uint16_t p1 = index;
	uint16_t p0 = -p1;
	
	if ( p1 == 0 )
	{
		return a;  // index is exactly on a byte boundary
	}
	
	/*
		The phase marker is between byte boundaries, so the "current" byte is
		really a combination of two consecutive samples.  Proportionally blend
		the two samples instead of taking just the one at the truncated index.
		
		This primarily affects notes with a sample playback rate under 1.0 --
		when two successive indices truncate to the same value, the sample gets
		repeated.  This creates a stairstep pattern in the waveform which is
		audible as a high-pitched tone.  Instead, proportionally blending the
		current sample with the next interpolates a new sample that lies along
		the wave, eliminating this stairstepping and the resulting artifact.
		(Other artifacts still remain, but they're much less pronounced.)
	*/
	
	return (a * p0 + b * p1) / 0x10000;
}

short ft_synth( sample_buffer& output, ft_buffer& rec, bool reset )
{
	if ( rec.duration <= 0 )
	{
		return -1;
	}
	
	if ( reset )
	{
		si1 = rec.sound1Phase << 16;
		si2 = rec.sound2Phase << 16;
		si3 = rec.sound3Phase << 16;
		si4 = rec.sound4Phase << 16;
	}
	
	--rec.duration;
	
	uint8_t* p = output.data;
	
	const bool has_1 = !! rec.sound1Rate;
	const bool has_2 = !! rec.sound2Rate;
	const bool has_3 = !! rec.sound3Rate;
	const bool has_4 = !! rec.sound4Rate;
	
	uint16_t n_voices = has_1 + has_2 + has_3 + has_4;
	
	if ( n_voices == 0 )
	{
		return 0;
	}
	
	for ( int i = 0;  i < sizeof output.data;  ++i )
	{
		int16_t sum = has_1 * sample( rec.sound1Wave, si1 )
		            + has_2 * sample( rec.sound2Wave, si2 )
		            + has_3 * sample( rec.sound3Wave, si3 )
		            + has_4 * sample( rec.sound4Wave, si4 );
		
		*p++ = sum / n_voices + 0x80;
		
		si1 += rec.sound1Rate;
		si2 += rec.sound2Rate;
		si3 += rec.sound3Rate;
		si4 += rec.sound4Rate;
	}
	
	return sizeof output.data;
}
