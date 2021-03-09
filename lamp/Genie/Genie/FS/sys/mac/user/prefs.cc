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

// mac-sys-utils
#include "mac_sys/has/FindFolder.hh"
#include "mac_sys/has/MFS_only.hh"

// mac-file-utils
#include "mac_file/parent_directory.hh"
#include "mac_file/system_file.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/methods/link_method_set.hh"
#include "vfs/methods/node_method_set.hh"

// relix-kernel
#include "relix/api/root.hh"

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
	
	static inline
	VRefNum_DirID system_folder()
	{
		using namespace mac::file;
		
		return parent_directory( system_file() );
	}
	
	static
	VRefNum_DirID GetPrefsFolder()
	{
		VRefNum_DirID result;
		
		if ( mac::sys::has_FindFolder() )
		{
			if ( FindPreferencesFolder( kUserDomain,   result ) )  goto done;
			if ( FindPreferencesFolder( kOnSystemDisk, result ) )  goto done;
		}
		
		return system_folder();
		
	done:
		
		return result;
	}
	
	
	static vfs::node_ptr mac_user_prefs_resolve( const vfs::node* that )
	{
		if ( mac::sys::has_MFS_only() )
		{
			return relix::root();
		}
		
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
