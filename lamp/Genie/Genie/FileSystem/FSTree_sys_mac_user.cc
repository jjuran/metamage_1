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
	
	static FSTreePtr Name_Factory( const FSTreePtr&    parent,
	                               const std::string&  name )
	{
		typedef FSTree_QueryFile< sys_mac_user_name_Query > QueryFile;
		
		return MakeFSTree( new QueryFile( parent, name ) );
	}
	
	void FSTree_sys_mac_user::Init()
	{
		try
		{
			CheckForHomeFolder();
			
			Map( "home", &Singleton_Factory< FSTree_sys_mac_user_home > );
		}
		catch ( ... )
		{
		}
		
		Map( "name", &Name_Factory );
	}
	
}

