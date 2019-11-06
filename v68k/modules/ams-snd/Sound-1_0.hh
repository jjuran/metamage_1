/*
	Sound-1_0.hh
	------------
*/

#ifndef SOUND10_HH
#define SOUND10_HH

struct SndChannel;
struct SndListResource;

typedef unsigned char Boolean;

pascal short SndPlay_patch( SndChannel* c, SndListResource** h, Boolean async );

#endif
