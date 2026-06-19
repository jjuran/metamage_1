/*
	FSCatInfo.hh
	------------
*/

#ifndef VFS_NODE_TYPES_FSCATINFO_HH
#define VFS_NODE_TYPES_FSCATINFO_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif


namespace vfs
{
	
#ifdef __APPLE__
	
	const FSCatalogInfoBitmap bitmap_for_GetFInfo
		= kFSCatInfoNodeID
		| kFSCatInfoCreateDate
		| kFSCatInfoContentMod
		| kFSCatInfoFinderInfo
		| kFSCatInfoDataSizes
		| kFSCatInfoRsrcSizes;
	
	const FSCatalogInfoBitmap bitmap_for_SetFInfo
		= kFSCatInfoCreateDate
		| kFSCatInfoContentMod
		| kFSCatInfoFinderInfo;
	
	/*
		TODO:  If info_bits is ever used in more than
		one translation unit, define it out-of-header.
	*/
	
	static const FSCatalogInfoBitmap info_bits[] =
	{
		0,
		kFSCatInfoFinderInfo,
		kFSCatInfoFinderInfo,
		bitmap_for_GetFInfo,
		bitmap_for_SetFInfo,
	};
	
	inline
	FSPermissionInfo& get( FSPermissionInfo& permissions )
	{
		return permissions;
	}
	
	inline
	FSPermissionInfo& get( UInt32* permissions )
	{
		return *(FSPermissionInfo*) permissions;
	}
	
	inline
	FSPermissionInfo& permissions( FSCatalogInfo& info )
	{
		return get( info.permissions );
	}
	
#endif  // #ifdef __APPLE__
	
}

#endif
