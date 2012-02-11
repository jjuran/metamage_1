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

// Genie
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/link_method_set.hh"
#include "Genie/FS/node_method_set.hh"


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
	
	
	static FSTreePtr mac_user_prefs_resolve( const FSTree* node )
	{
		return FSTreeFromFSDirSpec( GetPrefsFolder(), false );
	}
	
	static const link_method_set mac_user_prefs_link_methods =
	{
		NULL,
		&mac_user_prefs_resolve
	};
	
	static const node_method_set mac_user_prefs_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&mac_user_prefs_link_methods
	};
	
	FSTreePtr New_FSTree_sys_mac_user_prefs( const FSTreePtr&     parent,
	                                         const plus::string&  name,
	                                         const void*          args )
	{
		return new FSTree( parent, name, S_IFLNK | 0777, &mac_user_prefs_methods );
	}
	
}

