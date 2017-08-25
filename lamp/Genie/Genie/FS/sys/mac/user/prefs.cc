/*
	Genie/FS/sys/mac/user/prefs.cc
	------------------------------
*/

#include "Genie/FS/sys/mac/user/prefs.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FOLDERS__
#include <Folders.h>
#endif

// POSIX
#include <sys/stat.h>

// mac-types
#include "mac_types/VRefNum_DirID.hh"

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/methods/link_method_set.hh"
#include "vfs/methods/node_method_set.hh"

// Genie
#include "Genie/FS/FSSpec.hh"


namespace Genie
{
	
	using mac::types::VRefNum_DirID;
	
	
	static inline
	bool FindPreferencesFolder( short domain, VRefNum_DirID& result )
	{
		OSErr err;
		err = FindFolder( domain,
		                  kPreferencesFolderType,
		                  false,
		                  &result.vRefNum,
		                  &result.dirID );
		
		return err == noErr;
	}
	
	static
	VRefNum_DirID GetPrefsFolder()
	{
		VRefNum_DirID result;
		
		if ( FindPreferencesFolder( kUserDomain,   result ) )  goto done;
		if ( FindPreferencesFolder( kOnSystemDisk, result ) )  goto done;
		
		Mac::ThrowOSStatus( fnfErr );
		
	done:
		
		return result;
	}
	
	
	static vfs::node_ptr mac_user_prefs_resolve( const vfs::node* that )
	{
		return FSTreeFromFSDirSpec( GetPrefsFolder() );
	}
	
	static const vfs::link_method_set mac_user_prefs_link_methods =
	{
		NULL,
		&mac_user_prefs_resolve
	};
	
	static const vfs::node_method_set mac_user_prefs_methods =
	{
		NULL,
		NULL,
		&mac_user_prefs_link_methods
	};
	
	vfs::node_ptr New_FSTree_sys_mac_user_prefs( const vfs::node*     parent,
	                                             const plus::string&  name,
	                                             const void*          args )
	{
		return new vfs::node( parent, name, S_IFLNK | 0777, &mac_user_prefs_methods );
	}
	
}
