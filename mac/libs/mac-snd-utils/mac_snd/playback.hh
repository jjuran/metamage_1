/*
	playback.hh
	-----------
*/

#ifndef MACSND_PLAYBACK_HH
#define MACSND_PLAYBACK_HH

// SoundDriver
#include "SoundDriver/SoundDriver.h"


namespace mac {
namespace snd {
	
	short play_sound( const void* buffer, long size );
	
	inline
	short play( const SWSynthRec& rec, unsigned size )
	{
		return play_sound( &rec, size );
	}
	
	inline
	short play( const FTSynthRec& rec, unsigned size = sizeof (FTSynthRec) )
	{
		return play_sound( &rec, size );
	}
	
	inline
	short play( const FFSynthRec& rec, unsigned size )
	{
		return play_sound( &rec, size );
	}

}
}

#endif
