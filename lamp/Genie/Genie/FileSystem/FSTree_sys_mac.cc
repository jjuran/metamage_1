/*	=================
 *	FSTree_sys_mac.cc
 *	=================
 */

#include "Genie/FileSystem/FSTree_sys_mac.hh"

// Genie
#include "Genie/FileSystem/FSTree_Generated.hh"
#include "Genie/FileSystem/FSTree_Property.hh"
#include "Genie/FileSystem/FSTree_sys_mac_desktop.hh"
#include "Genie/FileSystem/FSTree_sys_mac_errata.hh"
#include "Genie/FileSystem/FSTree_sys_mac_gdev.hh"
#include "Genie/FileSystem/FSTree_sys_mac_gestalt.hh"
#include "Genie/FileSystem/FSTree_sys_mac_keys.hh"
#include "Genie/FileSystem/FSTree_sys_mac_machine.hh"
#include "Genie/FileSystem/FSTree_sys_mac_name.hh"
#include "Genie/FileSystem/FSTree_sys_mac_proc.hh"
#include "Genie/FileSystem/FSTree_sys_mac_rom.hh"
#if !TARGET_API_MAC_CARBON
#include "Genie/FileSystem/FSTree_sys_mac_adb.hh"
#include "Genie/FileSystem/FSTree_sys_mac_crm.hh"
#include "Genie/FileSystem/FSTree_sys_mac_drive.hh"
#include "Genie/FileSystem/FSTree_sys_mac_unit.hh"
#include "Genie/FileSystem/FSTree_sys_mac_xpram.hh"
#endif
#include "Genie/FileSystem/FSTree_sys_mac_user.hh"
#include "Genie/FileSystem/FSTree_sys_mac_vol.hh"
//#include "Genie/FileSystem/FSTree_sys_mac_window.hh"


namespace Genie
{
	
	template < class Property >
	static FSTreePtr Property_Factory( const FSTreePtr&    parent,
	                                   const std::string&  name )
	{
		return FSTreePtr( new FSTree_Property( parent,
		                                       name,
		                                       &Property::Read ) );
	}
	
	template < class Property >
	static FSTreePtr Generated_Factory( const FSTreePtr&    parent,
	                                    const std::string&  name )
	{
		return FSTreePtr( new FSTree_Generated( parent,
		                                        name,
		                                        &Property::Read ) );
	}
	
	const FSTree_Premapped::Mapping sys_mac_Mappings[] =
	{
		{ "vol",    &Basic_Factory< FSTree_sys_mac_vol    > },
		{ "gdev",   &Basic_Factory< FSTree_sys_mac_gdev   > },
		{ "proc",   &Basic_Factory< FSTree_sys_mac_proc   > },
		//{ "window", &Basic_Factory< FSTree_sys_mac_window > },
		
	#if !TARGET_API_MAC_CARBON
		
		{ "crm",  &Premapped_Factory< sys_mac_crm_Mappings > },
		
		{ "adb",   &Basic_Factory< FSTree_sys_mac_adb   > },
		{ "drive", &Basic_Factory< FSTree_sys_mac_drive > },
		{ "unit",  &Basic_Factory< FSTree_sys_mac_unit  > },
		
		{ "xpram", &Generated_Factory< sys_mac_xpram > },
		
	#endif
		
		{ "desktop", &Premapped_Factory< sys_mac_desktop_Mappings > },
		{ "errata",  &Premapped_Factory< sys_mac_errata_Mappings  > },
		{ "machine", &Premapped_Factory< sys_mac_machine_Mappings > },
		{ "user",    &Premapped_Factory< sys_mac_user_Mappings    > },
		
		{ "gestalt", &Basic_Factory< FSTree_sys_mac_gestalt > },
		
		{ "name", &Property_Factory< sys_mac_name > },
		{ "keys", &Generated_Factory< sys_mac_keys > },
		
		{ "rom", &Basic_Factory< FSTree_sys_mac_rom > },
		
		{ NULL, NULL }
		
	};
	
}

