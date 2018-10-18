/*
	parent_directory.cc
	-------------------
*/

#include "mac_file/parent_directory.hh"

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
	
	using types::VRefNum_DirID;
	
	
#if ! __LP64__
	
	VRefNum_DirID parent_directory( const VRefNum_DirID& dir )
	{
		CInfoPBRec pb;
		
		DirInfo& dirInfo = pb.dirInfo;
		
		short vRefNum = dir.vRefNum;
		
		dirInfo.ioNamePtr = NULL;
		dirInfo.ioVRefNum = vRefNum;
		dirInfo.ioDrDirID = dir.dirID;
		dirInfo.ioFDirIndex = -1;
		
		if ( OSStatus err = PBGetCatInfoSync( &pb ) )
		{
			vRefNum           = 0;
			dirInfo.ioDrParID = err;
		}
		
		VRefNum_DirID result = { vRefNum, dirInfo.ioDrParID };
		
		return result;
	}
	
#else
	
	void dummy()
	{
	}
	
#endif  // #if ! __LP64__
	
}
}
