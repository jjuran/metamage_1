/*	======================
 *	FSTree_sys_mac_user.cc
 *	======================
 */

#include "Genie/FileSystem/FSTree_sys_mac_user.hh"

// Genie
#include "Genie/FileSystem/FSTree_QueryFile.hh"
#include "Genie/FileSystem/FSTree_sys_mac.hh"
#include "Genie/FileSystem/FSTree_sys_mac_user_home.hh"
#include "Genie/FileSystem/FSTree_sys_mac_user_name.hh"


namespace Genie
{
	
	void FSTree_sys_mac_user::Init()
	{
		try
		{
			CheckForHomeFolder();
			
			MapSingleton< FSTree_sys_mac_user_home >();
		}
		catch ( ... )
		{
		}
		
		Map( FSTreePtr( new FSTree_QueryFile< sys_mac_user_name_Query >( shared_from_this(),
		                                                                 "name" ) ) );
	}
	
	FSTreePtr FSTree_sys_mac_user::Parent() const
	{
		return GetSingleton< FSTree_sys_mac >();
	}
	
}

