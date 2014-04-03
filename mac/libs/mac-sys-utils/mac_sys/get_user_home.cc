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
#ifndef __GESTALT__
#include <Gestalt.h>
#endif

// mac-types
#include "mac_types/MultipleUsersState.hh"


namespace mac {
namespace sys {
	
	using mac::types::VRefNum_DirID;
	using mac::types::MultipleUsersState;
	
	
	mac::types::VRefNum_DirID get_user_home()
	{
		mac::types::VRefNum_DirID result = { 0 };
		
		OSErr err = ::FindFolder( kOnAppropriateDisk,
		                          kCurrentUserFolderType,
		                          kDontCreateFolder,
		                          &result.vRefNum,
		                          &result.dirID );
		
		if ( err != noErr )
		{
			MultipleUsersState** handle;
			
			err = ::Gestalt( gestaltMultipleUsersState, (SInt32*) &handle );
			
			if ( err == noErr )
			{
				MultipleUsersState* state = *handle;
				
				const short version = state->giVersion;
				
				if ( version > 1 )
				{
					return (const VRefNum_DirID&) state->giDocsVRefNum;
				}
			}
		}
		
		return result;
	}
	
}
}
