/*	======================
 *	FSTree_sys_mac_user.cc
 *	======================
 */

#include "Genie/FileSystem/FSTree_sys_mac_user.hh"

// Nucleus
#include "Nucleus/ArrayContainerFunctions.h"

// Genie
#include "Genie/FileSystem/FSTree_QueryFile.hh"
#include "Genie/FileSystem/FSTree_sys_mac.hh"
#include "Genie/FileSystem/FSTree_sys_mac_user_home.hh"
#include "Genie/FileSystem/FSTree_sys_mac_user_name.hh"


namespace Genie
{
	
	namespace NN = Nucleus;
	
	
	static FSTreePtr Name_Factory( const FSTreePtr&    parent,
	                               const std::string&  name )
	{
		typedef FSTree_QueryFile< sys_mac_user_name_Query > QueryFile;
		
		return MakeFSTree( new QueryFile( parent, name ) );
	}
	
	static FSTree_sys_mac_user::Mapping sys_mac_user_Mappings[] =
	{
		{ "home", &Singleton_Factory< FSTree_sys_mac_user_home  > },
		
		{ "name", &Name_Factory },
	};
	
	void FSTree_sys_mac_user::Init()
	{
		AddMappings( sys_mac_user_Mappings, NN::ArrayEnd( sys_mac_user_Mappings ) );
	}
	
}

