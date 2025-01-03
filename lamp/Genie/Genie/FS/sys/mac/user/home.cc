/*
	Genie/FS/sys/mac/user/home.cc
	-----------------------------
*/

#include "Genie/FS/sys/mac/user/home.hh"

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

// mac-sys-utils
#include "mac_sys/get_user_home.hh"
#include "mac_sys/has/FindFolder.hh"
#include "mac_sys/has/MFS_only.hh"

// mac-file-utils
#include "mac_file/parent_directory.hh"
#include "mac_file/program_file.hh"

// MacIO
#include "MacIO/GetCatInfo_Sync.hh"

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
	using mac::sys::has_FindFolder;
	using mac::file::parent_directory;
	using mac::file::program_file;
	
	
	static inline
	bool FindUsersFolder( short vRefNum, VRefNum_DirID& result )
	{
		OSErr err;
		err = FindFolder( vRefNum,
		                  kUsersFolderType,
		                  false,
		                  &result.vRefNum,
		                  &result.dirID );
		
		return err == noErr;
	}
	
	static
	VRefNum_DirID GetUsersFolder( FSVolumeRefNum vRefNum )
	{
		VRefNum_DirID result;
		
		if ( has_FindFolder()  &&  FindUsersFolder( vRefNum, result ) )
		{
			return result;
		}
		
		CInfoPBRec cInfo;
		
		const FSSpec users = { vRefNum, fsRtDirID, "\p" "Users" };
		
		MacIO::GetCatInfo< MacIO::Throw_All >( cInfo, users );
		
		return Dir_From_CInfo( cInfo );
	}
	
	static
	VRefNum_DirID FindUserHomeFolder()
	{
		VRefNum_DirID parent = parent_directory( program_file() );
		VRefNum_DirID users  = GetUsersFolder( parent.vRefNum );
		VRefNum_DirID child;
		
		child.vRefNum = parent.vRefNum;
		
		do
		{
			child.dirID = parent.dirID;
			
			parent = mac::file::parent_directory( child );
			
			if ( is_error( parent ) )
			{
				Mac::ThrowOSStatus( error( parent ) );
			}
		}
		while ( parent.dirID != users.dirID );
		
		return child;
	}
	
	
	static vfs::node_ptr mac_user_home_resolve( const vfs::node* that )
	{
		if ( mac::sys::has_MFS_only() )
		{
			return relix::root();
		}
		
		VRefNum_DirID folder = mac::sys::get_user_home();
		
		if ( is_error( folder ) )
		{
			folder = FindUserHomeFolder();
		}
		
		return node_from_dir( folder );
	}
	
	static const vfs::link_method_set mac_user_home_link_methods =
	{
		NULL,
		&mac_user_home_resolve
	};
	
	static const vfs::node_method_set mac_user_home_methods =
	{
		NULL,
		NULL,
		&mac_user_home_link_methods
	};
	
	vfs::node_ptr New_FSTree_sys_mac_user_home( const vfs::node*     parent,
	                                            const plus::string&  name,
	                                            const void*          args )
	{
		return new vfs::node( parent, name, S_IFLNK | 0777, &mac_user_home_methods );
	}
	
}
