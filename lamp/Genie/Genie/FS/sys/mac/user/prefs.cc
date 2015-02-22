/*
	Genie/FS/sys/mac/user/prefs.cc
	------------------------------
*/

#include "Genie/FS/sys/mac/user/prefs.hh"

// POSIX
#include <sys/stat.h>

// Nitrogen
#include "Nitrogen/Folders.hh"

// Io: MacFiles
#include "MacFiles/Classic.hh"

// vfs
#include "vfs/methods/link_method_set.hh"
#include "vfs/methods/node_method_set.hh"

// Genie
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	static Mac::FSDirSpec GetPrefsFolder()
	{
		try
		{
			return N::FindFolder( N::kUserDomain,
			                      N::kPreferencesFolderType,
			                      false );
		}
		catch ( ... )
		{
			return N::FindFolder( N::kOnSystemDisk,
			                      N::kPreferencesFolderType,
			                      false );
		}
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
		return new FSTree( parent, name, S_IFLNK | 0777, &mac_user_prefs_methods );
	}
	
}
