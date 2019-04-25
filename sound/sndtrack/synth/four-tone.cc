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
		uint8_t i1 = si1 >> 16;
		uint8_t i2 = si2 >> 16;
		uint8_t i3 = si3 >> 16;
		uint8_t i4 = si4 >> 16;
		
		int16_t sum = has_1 * (int8_t) (rec.sound1Wave[ i1 ] ^ 0x80)
		            + has_2 * (int8_t) (rec.sound2Wave[ i2 ] ^ 0x80)
		            + has_3 * (int8_t) (rec.sound3Wave[ i3 ] ^ 0x80)
		            + has_4 * (int8_t) (rec.sound4Wave[ i4 ] ^ 0x80);
		
		*p++ = sum / n_voices + 0x80;
		
		si1 += rec.sound1Rate;
		si2 += rec.sound2Rate;
		si3 += rec.sound3Rate;
		si4 += rec.sound4Rate;
	}
	
	return sizeof output.data;
}
