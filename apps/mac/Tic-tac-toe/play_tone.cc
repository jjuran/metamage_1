/*
	play_tone.cc
	------------
*/

#include "play_tone.hh"

// mac-snd-utils
#include "mac_snd/playback.hh"


void play_tone( unsigned short swCount )
{
	typedef unsigned short UInt16;
	
	const UInt16 n_notes = 1;
	
	UInt16 buffer[ 1 + 3 * (n_notes + 1) ];
	
	UInt16* p = buffer;
	
	*p++ = (UInt16) swMode;
	
	const UInt16 amplitude = 255;  // 0 - 255
	const UInt16 duration  =   6;  // 0 - 65535
	
	*p++ = swCount;
	*p++ = amplitude;
	*p++ = duration;
	
	*p++ = 0;
	*p++ = 0;
	*p++ = 0;
	
	const SWSynthRec& rec = *(SWSynthRec*) buffer;
	
	mac::snd::play( rec, sizeof buffer );
}
