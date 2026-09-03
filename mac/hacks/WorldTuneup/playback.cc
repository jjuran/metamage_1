/*
	playback.cc
	-----------
	
	World-building Tune-up INIT for Mac OS
	
	Copyright 2026, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	For "worlds" developed with World Builder.
	
*/

#include "playback.hh"

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif
#ifndef __SOUND__
#include <Sound.h>
#endif

// mac-glue-utils
#include "mac_glue/Memory.hh"

// WorldTuneup
#include "state.hh"


#pragma exceptions off


using mac::glue::GetHandleSize_raw;


enum
{
	_HUnlock = 0xA02A,
};

short MemErr : 0x0220;


struct ASND_header
{
	short vers;
	short count;
	
	Byte deltas[ 16 ];
};

static inline
PerAppState& get_state()
{
	return get_state( GetResource( 'CODE', 1 ) );
}

static
void play_ASND( Handle asnd_h : __A0 )
{
	OSErr err;
	
	Size size = GetHandleSize_raw( asnd_h ) - 0x14;
	
	if ( size <= 0 )
	{
		goto error;
	}
	
	Size n_packets = size;
	Size n_samples = n_packets * 2;
	
	PerAppState& state = get_state();
	
	Handle buffer = state.buffer;
	
	size = sizeof (SoundHeader) + n_samples;
	
	SetHandleSize( buffer, size );
	
	if ( MemErr )
	{
		goto error;
	}
	
	HLock( buffer );
	
	const ASND_header& asnd = *(const ASND_header*) *asnd_h;
	
	SoundHeader& sh = *(SoundHeader*) *buffer;
	
	Ptr samples = *buffer + sizeof sh;
	
	sh.samplePtr  = samples;
	sh.length     = n_samples;
	sh.sampleRate = rate11khz;
	sh.loopStart  = 0;
	sh.loopEnd    = 0;
	sh.encode     = 0;
	
	Ptr p = *asnd_h + 0x14;
	Ptr q = samples;
	
	short level = 128;
	
	do
	{
		Byte c = *p++;
		
		*q++ = level += asnd.deltas[ c >>  4 ];
		*q++ = level += asnd.deltas[ c & 0xF ];
	}
	while ( --n_packets > 0 );
	
	SndChannelPtr channel = state.channel;
	
	SndCommand playback = { bufferCmd,   0, (long) &sh    };
	SndCommand callback = { callBackCmd, 0, (long) buffer };
	
	short n = asnd.count;
	
	do
	{
		err = SndDoCommand( channel, &playback, true );
	}
	while ( err == noErr  &&  --n > 0 );
	
	err = SndDoCommand( channel, &callback, true );
	
	if ( err )
	{
		SndCommand flush = { flushCmd };
		SndCommand quiet = { quietCmd };
		
		SndDoImmediate( channel, &flush );
		SndDoImmediate( channel, &quiet );
		
		HUnlock( buffer );
		
	error:
		
		SoundActive = false;
	}
}

asm
void playback_patch()
{
	/*
		The 'ASND' resource handle is in A3.
		
		The World Builder runtime HLocks the 'ASND' resource,
		but we're going to decode the whole thing in one go.
	*/
	
	MOVEA.L  A3,A0
	
	_HUnlock
	
	JSR      play_ASND
	
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
}

/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
