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


#pragma exceptions off


namespace mac  {
namespace file {
	
#if ! __LP64__
	
	OSErr make_FSSpec( FSSpec& result, const types::VRefNum_DirID& dir )
	{
		CInfoPBRec pb;
		
		DirInfo& dirInfo = pb.dirInfo;
		
		short vRefNum = dir.vRefNum;
		
		result.name[ 0 ] = 0;
		
		dirInfo.ioNamePtr = result.name;
		dirInfo.ioVRefNum = vRefNum;
		dirInfo.ioDrDirID = dir.dirID;
		dirInfo.ioFDirIndex = -1;
		
		if ( OSErr err = PBGetCatInfoSync( &pb ) )
		{
			return err;
		}
		
		result.vRefNum = vRefNum;
		result.parID   = dirInfo.ioDrParID;
		
		return noErr;
	}
	
	types::FSSpec make_FSSpec( const types::VRefNum_DirID& dir )
	{
		types::FSSpec result;
		
		if ( OSStatus err = make_FSSpec( result, dir ) )
		{
			result.vRefNum = 0;
			result.parID   = err;
		}
		
		return result;
	}
	
#else
	
	void dummy()
	{
	}
	
#endif  // #if ! __LP64__
	
}
}
