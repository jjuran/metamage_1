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

// mac-sys-utils
#include "mac_sys/gestalt.hh"
#include "mac_sys/has/FindFolder.hh"


namespace mac {
namespace sys {
	
	using mac::types::VRefNum_DirID;
	
	
	static inline
	bool FindCurrentUserFolder( VRefNum_DirID& result )
	{
		OSErr err;
		err = FindFolder( kOnAppropriateDisk,
		                  kCurrentUserFolderType,
		                  kDontCreateFolder,
		                  &result.vRefNum,
		                  &result.dirID );
		
		return err == noErr;
	}
	
	static inline
	const VRefNum_DirID* get_MultiUsersDocsFolder()
	{
		enum
		{
			gestaltMultipleUsersState = 'mfdr',
		};
		
	#ifndef __APPLE__
		
		typedef MultiUserGestalt MultipleUsersState;
		
		if ( long got = gestalt( gestaltMultipleUsersState ) )
		{
			MultipleUsersState* state = *(MultipleUsersState**) got;
			
			if ( state->giVersion > 1 )
			{
				return (const VRefNum_DirID*) &state->giDocsVRefNum;
			}
		}
		
	#endif
		
		return NULL;
	}
	
	mac::types::VRefNum_DirID get_user_home()
	{
		VRefNum_DirID result;
		
		if ( has_FindFolder()  &&  FindCurrentUserFolder( result ) )
		{
			return result;
		}
		
		if ( const VRefNum_DirID* documents = get_MultiUsersDocsFolder() )
		{
			return *documents;
		}
		
		result.vRefNum = 0;
		result.dirID   = 0;
		
		return result;
	}
	
}
}
