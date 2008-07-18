/*	=================
 *	FSTree_sys_mac.cc
 *	=================
 */

#include "Genie/FileSystem/FSTree_sys_mac.hh"

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
	
	static FSTreePtr Name_Factory( const FSTreePtr&    parent,
	                               const std::string&  name )
	{
		typedef FSTree_QueryFile< sys_mac_name_Query > QueryFile;
		
		return MakeFSTree( new QueryFile( parent, name ) );
	}
	
	void FSTree_sys_mac::Init()
	{
		MapSingleton< FSTree_sys_mac_vol     >();
		MapSingleton< FSTree_sys_mac_proc    >();
		MapSingleton< FSTree_sys_mac_window  >();
		
	#if !TARGET_API_MAC_CARBON
		
		MapSingleton< FSTree_sys_mac_crm     >();
		MapSingleton< FSTree_sys_mac_unit    >();
		
	#endif
		
		MapSingleton< FSTree_sys_mac_user    >();
		MapSingleton< FSTree_sys_mac_gestalt >();
		
		Map( "name", &Name_Factory );
		
		try
		{
			GetROMSize();
			
			MapSingleton< FSTree_sys_mac_rom >();
		}
		catch ( ... )
		{
		}
	}
	
	FSTreePtr FSTree_sys_mac::Parent() const
	{
		return GetSingleton< FSTree_sys >();
	}
	
}

