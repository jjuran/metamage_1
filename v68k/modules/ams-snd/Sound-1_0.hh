/*
	Sound-1_0.hh
	------------
*/

#ifndef SOUND10_HH
#define SOUND10_HH

struct SndChannel;
struct SndCommand;
struct SndListResource;

typedef unsigned char Boolean;

typedef pascal void (*SndCallBackUPP)( SndChannel*, SndCommand* );

pascal short SndPlay_patch( SndChannel* c, SndListResource** h, Boolean async );

pascal short SndNewChannel_patch( SndChannel**, short, long, SndCallBackUPP );

pascal short SndDisposeChannel_patch( SndChannel** c );

#endif
