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
		Map( "vol",    &Singleton_Factory< FSTree_sys_mac_vol    > );
		Map( "proc",   &Singleton_Factory< FSTree_sys_mac_proc   > );
		Map( "window", &Singleton_Factory< FSTree_sys_mac_window > );
		
	#if !TARGET_API_MAC_CARBON
		
		Map( "crm",  &Singleton_Factory< FSTree_sys_mac_crm  > );
		Map( "unit", &Singleton_Factory< FSTree_sys_mac_unit > );
		
	#endif
		
		Map( "user",    &Singleton_Factory< FSTree_sys_mac_user    > );
		Map( "gestalt", &Singleton_Factory< FSTree_sys_mac_gestalt > );
		
		Map( "name", &Name_Factory );
		
		try
		{
			GetROMSize();
			
			Map( "rom", &Singleton_Factory< FSTree_sys_mac_rom > );
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

