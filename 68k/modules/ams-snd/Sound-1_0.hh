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

typedef short OSErr;

typedef pascal void (*SndCallBackUPP)( SndChannel*, SndCommand* );

pascal OSErr SndPlay_patch( SndChannel* c, SndListResource** h, Boolean async );

pascal OSErr SndNewChannel_patch( SndChannel**, short, long, SndCallBackUPP );

pascal OSErr SndDisposeChannel_patch( SndChannel* chan, Boolean quietNow );

pascal OSErr SndDoCommand_patch( SndChannel* c, SndCommand* cmd, Boolean nblk );

pascal OSErr SndDoImmediate_patch( SndChannel* chan, SndCommand* cmd );

#endif
