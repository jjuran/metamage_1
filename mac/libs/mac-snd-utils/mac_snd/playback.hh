/*
	playback.hh
	-----------
*/

#ifndef MACSND_PLAYBACK_HH
#define MACSND_PLAYBACK_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef MAC_OS_X_VERSION_10_5
#ifndef __DEVICES__
#include <Devices.h>
#endif
#endif


namespace mac {
namespace snd {

enum
{
	kSoundDriverRefNum = -4,
};

inline
OSErr play_now_async( ParamBlockRec& pb, Ptr p, Size n )
{
	pb.ioParam.ioRefNum   = kSoundDriverRefNum;
	pb.ioParam.ioBuffer   = p;
	pb.ioParam.ioReqCount = n;
	
#if CALL_NOT_IN_CARBON
	
	PBKillIOSync( &pb );  // this sync call clears ioCompletion
	
#endif
	
	return PBWriteAsync( &pb );
}

inline
OSErr full_stop( ParamBlockRec& pb )
{
	pb.ioParam.ioRefNum = kSoundDriverRefNum;
	
#if CALL_NOT_IN_CARBON
	
	return PBKillIOSync( &pb );
	
#endif
	
	return noErr;
}

}
}

#endif
