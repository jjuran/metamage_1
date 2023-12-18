/*
	rate.cc
	-------
*/

#include "rate.hh"

// Standard C
#ifndef __MC68K__
#include <stdint.h>
#endif

// math
#include "math/integer.hh"


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

#ifdef __MC68K__

using math::integer::long_multiply;

asm
FTRate rate_from_midi_note( MIDI_note midi_note : __D0 )
{
	/*
		Save D3 so we can use it as a loop counter.
		
		Also allocate two longwords at the top of the stack
		(for long_multiply()'s result) by "saving" D1 and D2.
	*/
	
	LINK     A6,#0
	MOVEM.L  D1-D3,-(SP)
	
	/*
		Divide the MIDI note value into octave (low word) and key (high word).
		Then swap the words so that the key (the number of half steps above C)
		is in the low word.
	*/
	
	MOVE.L   D0,D3
	MOVEQ    #12,D0
	DIVU.W   D0,D3
	SWAP     D3
	
	MOVE.L   #C7_FTRate,D0
	BRA.S    loop_entry
	
	/*
		For each half-step above C, multiply by the 12th root of 2.
		Since it's a 16.16 fixed-point value, we need the result shifted
		right by 16 bits.  Since the result is in memory and always fits
		in 32 bits, we can read bits 16-47 (as a longword) back into D0.
	*/
	
loop:
	MOVEA.L  SP,A0
	MOVE.L   #fixed_12th_root,D1
	
	JSR      long_multiply
	
	MOVE.L   2(SP),D0
	
loop_entry:
	DBRA.S   D3,loop
	
	/*
		Return the octave number to the low word of D3.
		
		Subtract the octave number from eight (not seven, because it's
		biased by one) to get the octave difference, and shift the rate
		right by that many bits in order to drop that number of octaves.
	*/
	
	SWAP     D3
	MOVEQ    #8,D1
	SUB.W    D3,D1
	LSR.L    D1,D0
	
	MOVEM.L  (SP)+,D1-D3
	UNLK     A6
	RTS
}

#else

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

#endif
