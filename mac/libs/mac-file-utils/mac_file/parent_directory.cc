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
	
#endif  // #if ! __LP64__
	
	types::VRefNum_DirID parent_directory( const FSRef& file )
	{
		FSSpec spec;
		if ( OSStatus err = FSGetCatalogInfo( &file, 0, 0, 0, &spec, 0 ) )
		{
			VRefNum_DirID error = { 0, err };
			return error;
		}
		
		return parent_directory( spec );
	}
	
}
}
