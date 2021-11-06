/*
	rate.cc
	-------
*/

#include "rate.hh"

// Standard C
#include <stdint.h>


enum
{
	tones_per_octave = 12u,
	
	/*
		This is the four-tone synth rate given in Inside Macintosh,
		Volume II for C7, the note three octaves above middle C.
	*/
	
	C7_FTRate = 0x1812eb,
	
	/*
		We approximate the 12th root of 2 with the fraction 69433 / 65536,
		which we express below in 16.16 fixed point as merely 69433.
	*/
	
	fixed_12th_root = 69433,
};

FTRate rate_from_midi_note( MIDI_note midi_note )
{
	uint64_t rate = C7_FTRate;
	
	int octave1 = midi_note / tones_per_octave;  // MIDI octave / SPN octave + 1
	int n_steps = midi_note % tones_per_octave;
	
	while ( n_steps-- > 0 )
	{
		rate *= fixed_12th_root;
		rate /= 65536u;
	}
	
	/*
		Convert from SPN octave 7 to the requested octave, by
		bitshifting right one bit per octave lower.  Since octave1
		is the requested octave plus one, (due to originating from
		a MIDI note value), we subtract it from eight, not seven.
	*/
	
	return (uint32_t) rate >> (8 - octave1);
}
