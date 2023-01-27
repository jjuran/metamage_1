/*
	fourtone.hh
	-----------
*/

#ifndef FOURTONE_HH
#define FOURTONE_HH

typedef unsigned long UnsignedFixed;

struct FTSoundRec;

struct ToneSpec
{
	UnsignedFixed rate;
	UnsignedFixed phase;
};

void fourtone_fill( FTSoundRec* sndRec, ToneSpec* prev, short* p, bool last );

#endif
