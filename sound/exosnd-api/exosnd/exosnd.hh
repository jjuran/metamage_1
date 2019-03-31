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
#else
	// For non-Mac, the exact type doesn't matter as long as the length fits.
	typedef uint32_t Phase;
#endif

enum
{
	basic_domain = 0x0101,
	admin_domain = 0x4A4A,
	sound_domain = 0x4B4B,
};

enum
{
	full_stop = 0xFFFF,  // basic
	
	ftMode_flat_buffer = ftMode | 0x0100,  // sound
	ftMode_flat_update = ftMode | 0x0200,  // admin
};

struct FTSynthRec_flat_header
{
	short  mode;
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
