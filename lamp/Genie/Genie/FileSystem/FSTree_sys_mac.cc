/*	=================
 *	FSTree_sys_mac.cc
 *	=================
 */

#include "Genie/FileSystem/FSTree_sys_mac.hh"

// Nucleus
#include "Nucleus/ArrayContainerFunctions.h"

// Genie
#include "Genie/FileSystem/FSTree_QueryFile.hh"
#include "Genie/FileSystem/FSTree_sys.hh"
#include "Genie/FileSystem/FSTree_sys_mac_gestalt.hh"
#include "Genie/FileSystem/FSTree_sys_mac_name.hh"
#include "Genie/FileSystem/FSTree_sys_mac_proc.hh"
#include "Genie/FileSystem/FSTree_sys_mac_rom.hh"
#if !TARGET_API_MAC_CARBON
#include "Genie/FileSystem/FSTree_sys_mac_crm.hh"
#include "Genie/FileSystem/FSTree_sys_mac_unit.hh"
#endif
#include "Genie/FileSystem/FSTree_sys_mac_user.hh"
#include "Genie/FileSystem/FSTree_sys_mac_vol.hh"
#include "Genie/FileSystem/FSTree_sys_mac_window.hh"


namespace Genie
{
	
	namespace NN = Nucleus;
	
	
	static FSTreePtr Name_Factory( const FSTreePtr&    parent,
	                               const std::string&  name )
	{
		typedef FSTree_QueryFile< sys_mac_name_Query > QueryFile;
		
		return MakeFSTree( new QueryFile( parent, name ) );
	}
	
	static FSTree_Functional_Singleton::Mapping sys_mac_Mappings[] =
	{
		{ "vol",    &Singleton_Factory< FSTree_sys_mac_vol    > },
		{ "proc",   &Singleton_Factory< FSTree_sys_mac_proc   > },
		{ "window", &Singleton_Factory< FSTree_sys_mac_window > },
		
	#if !TARGET_API_MAC_CARBON
		
		{ "crm",  &Singleton_Factory< FSTree_sys_mac_crm  > },
		{ "unit", &Singleton_Factory< FSTree_sys_mac_unit > },
		
	#endif
		
		{ "user",    &Singleton_Factory< FSTree_sys_mac_user    > },
		{ "gestalt", &Singleton_Factory< FSTree_sys_mac_gestalt > },
		
		{ "name", &Name_Factory },
		
		{ "rom", &Singleton_Factory< FSTree_sys_mac_rom > }
		
	};
	
	void FSTree_sys_mac::Init()
	{
		AddMappings( sys_mac_Mappings, NN::ArrayEnd( sys_mac_Mappings ) );
	}
	
}

