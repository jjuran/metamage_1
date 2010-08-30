/*
	Genie/FS/sys/mac/user/home.cc
	-----------------------------
*/

#include "Genie/FS/sys/mac/user/home.hh"

// Nitrogen
#include "Nitrogen/Folders.hh"

// Io: MacFiles
#include "MacFiles/Classic.hh"

// MacIO
#include "MacIO/FSMakeFSSpec_Sync.hh"
#include "MacIO/GetCatInfo_Sync.hh"

// Genie
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/ResolvableSymLink.hh"
#include "Genie/Utilities/AsyncIO.hh"
#include "Genie/Utilities/GetAppFolder.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	class FSTree_sys_mac_user_home : public FSTree_ResolvableSymLink
	{
		public:
			FSTree_sys_mac_user_home( const FSTreePtr&     parent,
			                          const plus::string&  name )
			:
				FSTree_ResolvableSymLink( parent, name )
			{
			}
			
			FSTreePtr ResolveLink() const;
	};
	
	
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
		try
		{
			return N::FindFolder( N::kOnAppropriateDisk, N::kCurrentUserFolderType, false );
		}
		catch ( ... )
		{
		}
		
		return FindUserHomeFolder();
	}
	
	
	FSTreePtr FSTree_sys_mac_user_home::ResolveLink() const
	{
		return FSTreeFromFSDirSpec( GetUserHomeFolder(), false );
	}
	
	FSTreePtr New_FSTree_sys_mac_user_home( const FSTreePtr&     parent,
	                                        const plus::string&  name,
	                                        const void*          args )
	{
		return seize_ptr( new FSTree_sys_mac_user_home( parent, name ) );
	}
	
}

