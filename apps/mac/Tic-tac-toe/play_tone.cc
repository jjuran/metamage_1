/*
	play_tone.cc
	------------
*/

#include "play_tone.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef MAC_OS_X_VERSION_10_5
#ifndef __DEVICES__
#include <Devices.h>
#endif
#endif

// SoundDriver
#include "SoundDriver/SoundDriver.h"


enum
{
	kSoundDriverRefNum = -4,
};

static ParamBlockRec pb;

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
	
	IOParam& io = pb.ioParam;
	
	io.ioRefNum   = kSoundDriverRefNum;
	io.ioBuffer   = (Ptr) buffer;
	io.ioReqCount = sizeof buffer;
	
#if CALL_NOT_IN_CARBON
	
	PBKillIOSync( &pb );
	
#endif
	
	PBWriteAsync( &pb );
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
	#if CALL_NOT_IN_CARBON
		
		PBKillIOSync( &pb );
		
	#endif
	}
}
