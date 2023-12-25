/*
	SoundDriver.mac.cc
	------------------
*/

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// SoundDriver
#include "SoundDriver/SoundDriver.h"

// tap-out
#include "tap/test.hh"


#pragma exceptions off


static const unsigned n_tests = 3;


enum
{
	kSoundDriverRefNum = -4,
};

static FTSoundRec soundRec;

static const UInt16 zero_SW[] = { swMode, 0, 0, 0 };

static const UInt16 zero_FF[] = { ffMode, 1, 0 };  // rate = 1.0

static
void zero_duration()
{
	OSErr err;
	Size size;
	
	size = sizeof zero_SW;
	
	err = FSWrite( kSoundDriverRefNum, &size, zero_SW );
	
	EXPECT_EQ( err, noErr );
	
	
	size = sizeof zero_FF;
	
	err = FSWrite( kSoundDriverRefNum, &size, zero_FF );
	
	EXPECT_EQ( err, noErr );
	
	
	FTSynthRec synth;
	
	synth.mode = ftMode;
	synth.sndRec = &soundRec;
	
	size = sizeof synth;
	
	err = FSWrite( kSoundDriverRefNum, &size, &synth );
	
	EXPECT_EQ( err, noErr );
}

int main( int argc, char** argv )
{
	tap::start( "SoundDriver.mac", n_tests );
	
	zero_duration();
	
	return 0;
}
