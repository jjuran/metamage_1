/*
	volume_params.cc
	----------------
*/

#include "mac_sys/volume_params.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// Standard C
#include <string.h>


namespace mac {
namespace sys {
	
#if ! __LP64__
	
	OSErr get_volume_params( GetVolParmsInfoBuffer& buffer, short vRefNum )
	{
		HParamBlockRec pb;
		
		HIOParam& io = pb.ioParam;
		
		io.ioNamePtr  = NULL;
		io.ioVRefNum  = vRefNum;
		io.ioBuffer   = (Ptr) &buffer;
		io.ioReqCount = sizeof buffer;
		
		memset( &buffer, '\0', sizeof buffer );
		
		const OSErr err = PBHGetVolParmsSync( &pb );
		
		return err;
	}
	
	bool volume_is_on_server( short vRefNum )
	{
		GetVolParmsInfoBuffer buffer;
		
		return get_volume_params( buffer, vRefNum ) == noErr  &&  buffer.vMServerAdr != 0;
	}
	
#else
	
	int dummy;
	
#endif
	
}
}
