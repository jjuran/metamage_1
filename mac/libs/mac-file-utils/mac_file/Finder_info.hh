/*
	Finder_info.hh
	--------------
*/

#ifndef MACFILE_FINDERINFO_HH
#define MACFILE_FINDERINFO_HH

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

inline
OSErr get_Finder_info( const FSSpec& file, FInfo& info )
{
	return HGetFInfo( file.vRefNum, file.parID, file.name, &info );
}

inline
OSErr set_Finder_info( const FSSpec& file, const FInfo& info )
{
	return HSetFInfo( file.vRefNum, file.parID, file.name, &info );
}

inline
OSErr get_Finder_info( const FSRef& file, FSCatalogInfo& info )
{
	return FSGetCatalogInfo( &file, kFSCatInfoFinderInfo, &info, 0, 0, 0 );
}

inline
OSErr set_Finder_info( const FSRef& file, const FSCatalogInfo& info )
{
	return FSSetCatalogInfo( &file, kFSCatInfoFinderInfo, &info );
}

inline
UInt16& Finder_flags( FInfo& info )
{
	return info.fdFlags;
}

inline
UInt16 Finder_flags( const FInfo& info )
{
	return info.fdFlags;
}

inline
UInt16& Finder_flags( FSCatalogInfo& info )
{
	return ((FileInfo*) info.finderInfo)->finderFlags;
}

inline
UInt16 Finder_flags( const FSCatalogInfo& info )
{
	return ((const FileInfo*) info.finderInfo)->finderFlags;
}

}
}

#endif
