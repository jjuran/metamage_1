/*
	SoundDriver.h
	-------------
*/

#ifndef SOUNDDRIVER_SOUNDDRIVER_H
#define SOUNDDRIVER_SOUNDDRIVER_H

#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#elif defined( __MACOS__ )
	/*
		Use the same types as MacTypes.h, with no inclusion overhead.
		(They must match, or errors will occur if MacTypes.h /is/ included.)
	*/
	typedef unsigned char  Byte;
	typedef long           Fixed;
#else
	/*
		For non-Mac, the exact types don't matter as long as the length fits.
	*/
	#include <stdint.h>
	typedef uint8_t  Byte;
	typedef uint32_t Fixed;
#endif


#ifdef __cplusplus
extern "C" {
#endif

#ifdef __GNUC__
#pragma pack(push, 2)
#else
#pragma options align=packed
#endif

#ifndef __SOUND__

enum
{
	swMode = -1,  // square-wave synthesizer
	ftMode =  1,  // four-tone synthesizer
	ffMode =  0,  // free-form synthesizer
};

#endif

/*
	Square-wave synthesizer
*/

struct Tone
{
	unsigned short count, amplitude, duration;
};

typedef Tone Tones[ 5001 ];

struct SWSynthRec
{
	short  mode;
	Tones  triplets;
};

/*
	Four-tone synthesizer
*/

typedef Byte Wave[ 256 ], *WavePtr;

struct FTSoundRec
{
	short    duration;
	Fixed    sound1Rate;
	long     sound1Phase;
	Fixed    sound2Rate;
	long     sound2Phase;
	Fixed    sound3Rate;
	long     sound3Phase;
	Fixed    sound4Rate;
	long     sound4Phase;
	WavePtr  sound1Wave;
	WavePtr  sound2Wave;
	WavePtr  sound3Wave;
	WavePtr  sound4Wave;
};

struct FTSynthRec
{
	short        mode;
	FTSoundRec*  sndRec;
};

/*
	Free-form synthesizer
*/

typedef Byte FreeWave[ 30001 ];

struct FFSynthRec
{
	short     mode;
	Fixed     count;
	FreeWave  waveBytes;
};

#ifdef __GNUC__
#pragma pack(pop)
#else
#pragma options align=reset
#endif

#ifdef __cplusplus
}
#endif

#endif
