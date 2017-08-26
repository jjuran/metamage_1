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

// mac-types
#include "mac_types/MultipleUsersState.hh"

// mac-sys-utils
#include "mac_sys/gestalt.hh"


namespace mac {
namespace sys {
	
	using mac::types::VRefNum_DirID;
	using mac::types::MultipleUsersState;
	
	
	static inline
	MultipleUsersState** get_MultipleUsersState()
	{
		enum
		{
			gestaltMultipleUsersState = 'mfdr',
		};
		
		return (MultipleUsersState**) gestalt( gestaltMultipleUsersState );
	}
	
	mac::types::VRefNum_DirID get_user_home()
	{
		VRefNum_DirID result;
		
		OSErr err = ::FindFolder( kOnAppropriateDisk,
		                          kCurrentUserFolderType,
		                          kDontCreateFolder,
		                          &result.vRefNum,
		                          &result.dirID );
		
		if ( err != noErr )
		{
			if ( MultipleUsersState** handle = get_MultipleUsersState() )
			{
				MultipleUsersState* state = *handle;
				
				const short version = state->giVersion;
				
				if ( version > 1 )
				{
					return (const VRefNum_DirID&) state->giDocsVRefNum;
				}
			}
		}
		
		result.vRefNum = 0;
		result.dirID   = 0;
		
		return result;
	}
	
}
}
