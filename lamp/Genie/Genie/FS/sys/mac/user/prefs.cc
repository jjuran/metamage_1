/*
	Genie/FS/sys/mac/user/prefs.cc
	------------------------------
*/

#include "Genie/FS/sys/mac/user/prefs.hh"

// Nitrogen
#include "Nitrogen/Folders.hh"

// Io: MacFiles
#include "MacFiles/Classic.hh"

// Genie
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/ResolvableSymLink.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	class sys_mac_user_prefs : public FSTree_ResolvableSymLink
	{
		public:
			sys_mac_user_prefs( const FSTreePtr&     parent,
			                    const plus::string&  name )
			:
				FSTree_ResolvableSymLink( parent, name )
			{
			}
			
			FSTreePtr ResolveLink() const;
	};
	
	
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
	
	
	FSTreePtr sys_mac_user_prefs::ResolveLink() const
	{
		return FSTreeFromFSDirSpec( GetPrefsFolder(), false );
	}
	
	FSTreePtr New_FSTree_sys_mac_user_prefs( const FSTreePtr&     parent,
	                                         const plus::string&  name,
	                                         const void*          args )
	{
		return seize_ptr( new sys_mac_user_prefs( parent, name ) );
	}
	
}

