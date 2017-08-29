/*
	make_FSSpec.cc
	--------------
*/

#include "mac_file/make_FSSpec.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif


namespace mac  {
namespace file {
	
	types::FSSpec make_FSSpec( const types::VRefNum_DirID& dir )
	{
		types::FSSpec result;
		
		CInfoPBRec pb;
		
		DirInfo& dirInfo = pb.dirInfo;
		
		short vRefNum = dir.vRefNum;
		
		result.name[ 0 ] = 0;
		
		dirInfo.ioNamePtr = result.name;
		dirInfo.ioVRefNum = vRefNum;
		dirInfo.ioDrDirID = dir.dirID;
		dirInfo.ioFDirIndex = -1;
		
		if ( OSStatus err = PBGetCatInfoSync( &pb ) )
		{
			vRefNum           = 0;
			dirInfo.ioDrParID = err;
		}
		
		result.vRefNum = vRefNum;
		result.parID   = dirInfo.ioDrParID;
		
		return result;
	}
	
}
}
