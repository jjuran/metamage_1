/*
	Genie/FS/sys/mac/user/home.cc
	-----------------------------
*/

#include "Genie/FS/sys/mac/user/home.hh"

// POSIX
#include <sys/stat.h>

// sys-mac-utils
#include "mac_sys/get_user_home.hh"

// Nitrogen
#include "Nitrogen/Folders.hh"

// Io: MacFiles
#include "MacFiles/Classic.hh"

// MacIO
#include "MacIO/FSMakeFSSpec_Sync.hh"
#include "MacIO/GetCatInfo_Sync.hh"

// Genie
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/link_method_set.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/Utilities/AsyncIO.hh"
#include "Genie/Utilities/GetAppFolder.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	using mac::types::VRefNum_DirID;
	
	
	static N::FSDirSpec GetUsersFolder( N::FSVolumeRefNum vRefNum )
	{
		try
		{
			return N::FindFolder( vRefNum, N::kUsersFolderType, false );
		}
		catch ( ... )
		{
		}
		
		CInfoPBRec cInfo = { 0 };
		
		const FSSpec users = { vRefNum, fsRtDirID, "\p" "Users" };
		
		MacIO::GetCatInfo< FNF_Throws >( cInfo, users );
		
		return Dir_From_CInfo( cInfo );
	}
	
	static N::FSDirSpec FindUserHomeFolder()
	{
		N::FSDirSpec appFolder = GetAppFolder();
		
		N::FSDirSpec users = GetUsersFolder( appFolder.vRefNum );
		
		N::FSDirSpec parent = appFolder;
		N::FSDirSpec child;
		
		do
		{
			child = parent;
			
			parent = io::get_preceding_directory( MacIO::FSMakeFSSpec< FNF_Throws >( child, NULL ) );
		}
		while ( parent != users );
		
		return child;
	}
	
	static N::FSDirSpec GetUserHomeFolder()
	{
		VRefNum_DirID folder = mac::sys::get_user_home();
		
		if ( folder.vRefNum != 0 )
		{
			N::FSDirSpec result;
			
			result.vRefNum = Mac::FSVolumeRefNum( folder.vRefNum );
			result.dirID   = Mac::FSDirID       ( folder.dirID   );
			
			return result;
		}
		
		return FindUserHomeFolder();
	}
	
	
	static FSTreePtr mac_user_home_resolve( const FSTree* that )
	{
		return FSTreeFromFSDirSpec( GetUserHomeFolder() );
	}
	
	static const link_method_set mac_user_home_link_methods =
	{
		NULL,
		&mac_user_home_resolve
	};
	
	static const node_method_set mac_user_home_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&mac_user_home_link_methods
	};
	
	FSTreePtr New_FSTree_sys_mac_user_home( const FSTree*        parent,
	                                        const plus::string&  name,
	                                        const void*          args )
	{
		return new FSTree( parent, name, S_IFLNK | 0777, &mac_user_home_methods );
	}
	
}

