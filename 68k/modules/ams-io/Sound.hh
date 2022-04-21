/*
	Sound.hh
	--------
*/

#ifndef SOUND_HH
#define SOUND_HH

enum
{
	kSoundDriverRefNum = -4,
};

struct CntrlParam;
struct DCtlEntry;
struct IOParam;

typedef DCtlEntry DCE;

short Sound_open( short trap_word : __D1, IOParam* pb : __A0, DCE* : __A1 );

short Sound_prime( short trap_word : __D1, IOParam* pb : __A0, DCE* : __A1 );

short Sound_control( short trap_word : __D1, CntrlParam* : __A0, DCE* : __A1 );

#define Sound_status  NULL

short Sound_close( short trap_word : __D1, IOParam* pb : __A0, DCE* : __A1 );

pascal void SysBeep_patch( short duration );

#endif
