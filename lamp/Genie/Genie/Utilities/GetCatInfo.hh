/*
	GetCatInfo.hh
	-------------
*/

#ifndef GENIE_UTILITIES_GETCATINFO_HH
#define GENIE_UTILITIES_GETCATINFO_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif


namespace Genie
{

inline
OSErr
GetCatInfo_5( CInfoPBRec& cInfo, short vRefNum, long dirID, Byte* name, int i )
{
	DirInfo& dirInfo = cInfo.dirInfo;
	
	dirInfo.ioNamePtr = name;
	dirInfo.ioVRefNum = vRefNum;
	dirInfo.ioDrDirID = dirID;
	dirInfo.ioFDirIndex = i;
	
	return PBGetCatInfoSync( &cInfo );
}

inline
OSErr
GetCatInfo_name( CInfoPBRec& cInfo, short vRefNum, long dirID, Byte* name )
{
	return GetCatInfo_5( cInfo, vRefNum, dirID, name, -1 );
}

inline
OSErr
GetCatInfo( CInfoPBRec& cInfo, short vRefNum, long dirID, const Byte* name )
{
	return GetCatInfo_5( cInfo, vRefNum, dirID, (StringPtr) name, 0 );
}

inline
OSErr
GetCatInfo( CInfoPBRec& cInfo, const FSSpec& file )
{
	return GetCatInfo( cInfo, file.vRefNum, file.parID, file.name );
}

}

#endif
