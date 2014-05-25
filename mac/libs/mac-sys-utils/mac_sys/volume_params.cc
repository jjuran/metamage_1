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
	
	short get_volume_params( ::GetVolParmsInfoBuffer& buffer, short vRefNum )
	{
		HParamBlockRec pb = { 0 };
		
		HIOParam& io = pb.ioParam;
		
		io.ioVRefNum  = vRefNum;
		io.ioBuffer   = (char *) &buffer;
		io.ioReqCount = sizeof buffer;
		
		memset( &buffer, '\0', sizeof buffer );
		
		const OSErr err = ::PBHGetVolParmsSync( &pb );
		
		return err;
	}
	
}
}
