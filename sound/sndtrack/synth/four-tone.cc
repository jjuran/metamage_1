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

static inline
int8_t sample( Wave wave, Fixed index )
{
	uint8_t i = index >> 16;
	
	int8_t a = wave[ i ] ^ 0x80;
	
	return a;
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
