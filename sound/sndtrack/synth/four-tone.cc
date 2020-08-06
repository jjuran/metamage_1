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

static Fixed prevRate1;
static Fixed prevRate2;
static Fixed prevRate3;
static Fixed prevRate4;

static
int16_t sample( Wave wave, Fixed index )
{
	uint8_t i = index >> 16;
	uint8_t j = i + 1;
	
	int8_t a = wave[ i ] ^ 0x80;
	int8_t b = wave[ j ] ^ 0x80;
	
	uint16_t p1 = index;
	uint16_t p0 = -p1;
	
	if ( p1 == 0 )
	{
		return a << 8;  // index is exactly on a byte boundary
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
	
	return (a * p0 + b * p1) / 0x100;
}

static
void fill_from_wavetable( output_sample_t* p, Wave wave, Fixed& si, Fixed rate )
{
	if ( rate == 0 )
	{
		memset( p, '\0', sizeof (sample_buffer) );
		return;
	}
	
	for ( int i = 0;  i < samples_per_buffer;  ++i )
	{
		*p++ = sample( wave, si );
		
		si += rate;
	}
}

short ft_synth( sample_buffer& output, ft_buffer& rec, bool reset )
{
	if ( rec.duration <= 0 )
	{
		return -1;
	}
	
	if ( rec.sound1Rate  &&  ! prevRate1 )  si1 = rec.sound1Phase << 16;
	if ( rec.sound2Rate  &&  ! prevRate2 )  si2 = rec.sound2Phase << 16;
	if ( rec.sound3Rate  &&  ! prevRate3 )  si3 = rec.sound3Phase << 16;
	if ( rec.sound4Rate  &&  ! prevRate4 )  si4 = rec.sound4Phase << 16;
	
	prevRate1 = rec.sound1Rate;
	prevRate2 = rec.sound2Rate;
	prevRate3 = rec.sound3Rate;
	prevRate4 = rec.sound4Rate;
	
	--rec.duration;
	
	output_sample_t* p = output.data;
	
	const bool has_1 = !! rec.sound1Rate;
	const bool has_2 = !! rec.sound2Rate;
	const bool has_3 = !! rec.sound3Rate;
	const bool has_4 = !! rec.sound4Rate;
	
	uint16_t n_voices = has_1 + has_2 + has_3 + has_4;
	
	if ( n_voices == 0 )
	{
		return 0;
	}
	
	sample_buffer v1;
	sample_buffer v2;
	sample_buffer v3;
	sample_buffer v4;
	
	fill_from_wavetable( v1.data, rec.sound1Wave, si1, rec.sound1Rate );
	fill_from_wavetable( v2.data, rec.sound2Wave, si2, rec.sound2Rate );
	fill_from_wavetable( v3.data, rec.sound3Wave, si3, rec.sound3Rate );
	fill_from_wavetable( v4.data, rec.sound4Wave, si4, rec.sound4Rate );
	
	for ( int i = 0;  i < samples_per_buffer;  ++i )
	{
		int32_t sum = v1.data[ i ]
		            + v2.data[ i ]
		            + v3.data[ i ]
		            + v4.data[ i ];
		
		*p++ = sum / 4;
	}
	
	return samples_per_buffer;
}
