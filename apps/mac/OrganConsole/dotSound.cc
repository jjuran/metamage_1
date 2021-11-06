/*
	dotSound.cc
	-----------
*/

#include "dotSound.hh"

#if ! TARGET_API_MAC_CARBON

// Mac OS
#ifndef __DEVICES__
#include <Devices.h>
#endif

// SoundDriver
#include "SoundDriver/SoundDriver.h"

// Organ Console
#include "waveform.hh"


#pragma exceptions off


enum
{
	kSoundDriverRefNum = -4,
};

static FTSoundRec sndRec;
static FTSynthRec synthRec = { ftMode, &sndRec };

static inline
void fourtone_assign_wave( int channel, UInt8* normal_wave )
{
	*(&sndRec.sound1Phase + channel * 2) = 0;
	*(&sndRec.sound1Wave  + channel    ) = normal_wave;
}

static
void initialize()
{
	fourtone_assign_wave( 0, organ );
	fourtone_assign_wave( 1, organ );
	fourtone_assign_wave( 2, organ );
	fourtone_assign_wave( 3, organ );
}

void fourtone_amend_rate( int channel, FTRate rate )
{
	Fixed* slot = &sndRec.sound1Rate + channel * 2;
	
	*slot++ = rate;
	
	*slot = 0;  // reset phase
}

static ParamBlockRec param_block;

void fourtone_start( FTRate rate )
{
	OSErr err;
	
	IOParam& pb = param_block.ioParam;
	
	if ( pb.ioResult > 0 )
	{
		return;
	}
	
	sndRec.duration = 32767;
	
	sndRec.sound1Rate  = rate;
	sndRec.sound1Phase = 0;
	sndRec.sound2Rate  = 0;
	sndRec.sound2Phase = 0;
	sndRec.sound3Rate  = 0;
	sndRec.sound3Phase = 0;
	sndRec.sound4Rate  = 0;
	sndRec.sound4Phase = 0;
	
	pb.ioCompletion = NULL;
	pb.ioRefNum     = kSoundDriverRefNum;
	pb.ioBuffer     = (Ptr) &synthRec;
	pb.ioReqCount   = sizeof synthRec;
	
	err = PBWriteAsync( &param_block );
}

void fourtone_amend_submit()
{
	sndRec.duration = 32767;
}

void full_stop()
{
	KillIO( kSoundDriverRefNum );
}

static bool initialized = (initialize(), true);

#endif  // #if ! TARGET_API_MAC_CARBON
