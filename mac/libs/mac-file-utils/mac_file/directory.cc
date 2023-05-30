/*
	directory.cc
	------------
*/

#include "mac_file/directory.hh"

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
	
	VRefNum_DirID directory( const FSSpec& dir )
	{
		VRefNum_DirID result;
		
		result.vRefNum = 0;
		
		CInfoPBRec pb;
		
		DirInfo& dirInfo = pb.dirInfo;
		
		dirInfo.ioNamePtr = (StringPtr) dir.name;
		dirInfo.ioVRefNum = dir.vRefNum;
		dirInfo.ioDrDirID = dir.parID;
		dirInfo.ioFDirIndex = 0;
		
		if ( OSStatus err = PBGetCatInfoSync( &pb ) )
		{
			result.dirID = err;
		}
		else if ( ! (pb.hFileInfo.ioFlAttrib & kioFlAttribDirMask) )
		{
			result.dirID = errFSNotAFolder;
		}
		else
		{
			result.vRefNum = dir.vRefNum;
			result.dirID   = pb.dirInfo.ioDrDirID;
		}
		
		return result;
	}
	
#endif  // #if ! __LP64__
	
	types::VRefNum_DirID directory( const FSRef& file )
	{
		FSSpec spec;
		if ( OSStatus err = FSGetCatalogInfo( &file, 0, 0, 0, &spec, 0 ) )
		{
			VRefNum_DirID error = { 0, err };
			return error;
		}
		
		return directory( spec );
	}
	
}
}
