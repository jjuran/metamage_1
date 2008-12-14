/*	======================
 *	FSTree_sys_mac_user.cc
 *	======================
 */

#include "Genie/FileSystem/FSTree_sys_mac_user.hh"

// Genie
#include "Genie/FileSystem/FSTree_QueryFile.hh"
#include "Genie/FileSystem/FSTree_sys_mac_user_home.hh"
#include "Genie/FileSystem/FSTree_sys_mac_user_name.hh"


namespace Genie
{
	
	static FSTreePtr Name_Factory( const FSTreePtr&    parent,
	                               const std::string&  name )
	{
		typedef FSTree_QueryFile< sys_mac_user_name_Query > QueryFile;
		
		return FSTreePtr( new QueryFile( parent, name ) );
	}
	
	const Singleton_Mapping sys_mac_user_Mappings[] =
	{
		{ "home", &Singleton_Factory< FSTree_sys_mac_user_home  > },
		
		{ "name", &Name_Factory },
		
		{ NULL, NULL }
	};
	
}

