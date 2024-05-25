/*
	play_tone.cc
	------------
*/

#include "play_tone.hh"

// SoundDriver
#include "SoundDriver/SoundDriver.h"

// mac-snd-utils
#include "mac_snd/playback.hh"


static ParamBlockRec pb;

void play_tone( unsigned short swCount )
{
	typedef unsigned short UInt16;
	
	const UInt16 n_notes = 1;
	
	UInt16 buffer[ 1 + 3 * (n_notes + 1) ];
	
	UInt16* p = buffer;
	
	*p++ = (UInt16) swMode;
	
	const UInt16 amplitude = 127;  // 0 - 255
	const UInt16 duration  =   6;  // 0 - 65535
	
	*p++ = swCount;
	*p++ = amplitude;
	*p++ = duration;
	
	*p++ = 0;
	*p++ = 0;
	*p++ = 0;
	
	mac::snd::play_now_async( pb, (Ptr) buffer, sizeof buffer );
}

void silence()
{
	IOParam& io = pb.ioParam;
	
	/*
		If the ioRefNum field hasn't been assigned yet, then we
		never called play_tone() and there's nothing to silence.
	*/
	
	if ( io.ioRefNum )
	{
		mac::snd::full_stop( pb );
	}
}
