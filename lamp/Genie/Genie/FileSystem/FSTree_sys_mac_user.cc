/*	======================
 *	FSTree_sys_mac_user.cc
 *	======================
 */

#include "Genie/FileSystem/FSTree_sys_mac_user.hh"

// Genie
#include "Genie/FileSystem/FSTree_sys_mac.hh"
#include "Genie/FileSystem/FSTree_sys_mac_user_home.hh"


namespace Genie
{
	
	FSTree_sys_mac_user::FSTree_sys_mac_user()
	{
		try
		{
			CheckForHomeFolder();
			
			MapSingleton< FSTree_sys_mac_user_home >();
		}
		catch ( ... )
		{
		}
	}
	
	FSTreePtr FSTree_sys_mac_user::Parent() const
	{
		return GetSingleton< FSTree_sys_mac >();
	}
	
}

