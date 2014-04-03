/*
	get_user_home.cc
	----------------
*/

#include "mac_sys/get_user_home.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FOLDERS__
#include <Folders.h>
#endif


namespace mac {
namespace sys {
	
	mac::types::VRefNum_DirID get_user_home()
	{
		mac::types::VRefNum_DirID result = { 0 };
		
		OSErr err = ::FindFolder( kOnAppropriateDisk,
		                          kCurrentUserFolderType,
		                          kDontCreateFolder,
		                          &result.vRefNum,
		                          &result.dirID );
		
		return result;
	}
	
}
}
