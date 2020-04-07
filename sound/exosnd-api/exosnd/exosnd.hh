/*
	exosnd.hh
	---------
*/

#ifndef EXOSND_EXOSND_HH
#define EXOSND_EXOSND_HH

// SoundDriver
#include "SoundDriver/SoundDriver.h"


namespace exosnd
{
	
#ifdef __MACOS__
	// Use the same type as MacTypes.h, with no inclusion overhead.
	typedef long Phase;
	typedef long RecID;
#else
	// For non-Mac, the exact type doesn't matter as long as the length fits.
	typedef uint32_t Phase;
	typedef uint32_t RecID;
#endif

enum
{
	basic_domain = 0x0101,
	admin_domain = 0x4A4A,
	sound_domain = 0x4B4B,
};

enum
{
	switch_on = 0x0000,  // basic
	full_stop = 0x5858,  // basic ('XX')
	
	ftMode_flat_buffer = ftMode | 0x0100,  // sound
	ftMode_flat_update = ftMode | 0x0200,  // admin
};

#ifdef __GNUC__
#pragma pack(push, 2)
#else
#pragma options align=packed
#endif

struct FTSynthRec_flat_header
{
	short  mode;
	RecID  recID;        // This is the FTSoundRec buffer address
	short  duration;
	Fixed  sound1Rate;
	Phase  sound1Phase;
	Fixed  sound2Rate;
	Phase  sound2Phase;
	Fixed  sound3Rate;
	Phase  sound3Phase;
	Fixed  sound4Rate;
	Phase  sound4Phase;
};

#ifdef __GNUC__
#pragma pack(pop)
#else
#pragma options align=reset
#endif

typedef FTSynthRec_flat_header FTSynthRec_flat_update;

struct FTSynthRec_flat_buffer : FTSynthRec_flat_header
{
	Wave  sound1Wave;
	Wave  sound2Wave;
	Wave  sound3Wave;
	Wave  sound4Wave;
};

}  // namespace exosnd

#endif
