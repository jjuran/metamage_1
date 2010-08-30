/*
	Genie/FS/sys/mac.cc
	-------------------
*/

#include "Genie/FS/sys/mac.hh"

// Genie
#include "Genie/FS/FSTree_Generated.hh"
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/sys/mac/desktop.hh"
#include "Genie/FS/sys/mac/errata.hh"
#include "Genie/FS/sys/mac/event.hh"
#include "Genie/FS/sys/mac/gdev.hh"
#include "Genie/FS/sys/mac/gestalt.hh"
#include "Genie/FS/sys/mac/keys.hh"
#include "Genie/FS/sys/mac/machine.hh"
#include "Genie/FS/FSTree_sys_mac_name.hh"
#include "Genie/FS/FSTree_sys_mac_proc.hh"
#include "Genie/FS/FSTree_sys_mac_rom.hh"
#include "Genie/FS/sys_mac_tempmem.hh"
#include "Genie/FS/FSTree_sys_mac_thng.hh"
#if defined( __MACOS__ )  &&  !TARGET_API_MAC_CARBON
#include "Genie/FS/sys/mac/adb.hh"
#include "Genie/FS/sys/mac/crm.hh"
#include "Genie/FS/sys/mac/crsr.hh"
#include "Genie/FS/sys/mac/drive.hh"
#include "Genie/FS/FSTree_sys_mac_unit.hh"
#include "Genie/FS/FSTree_sys_mac_xpram.hh"
#endif
#include "Genie/FS/sys/mac/time.hh"
#include "Genie/FS/FSTree_sys_mac_user.hh"
#include "Genie/FS/sys_mac_vol.hh"
//#include "Genie/FS/FSTree_sys_mac_window.hh"


namespace Genie
{
	
	template < class Property >
	static FSTreePtr Property_Factory( const FSTreePtr&     parent,
	                                   const plus::string&  name,
	                                   const void*          args )
	{
		return New_FSTree_Property( parent,
		                            name,
		                            &Property::Read );
	}
	
	template < class Property >
	static FSTreePtr Generated_Factory( const FSTreePtr&     parent,
	                                    const plus::string&  name,
	                                    const void*          args )
	{
		return New_FSTree_Generated( parent,
		                             name,
		                             &Property::Read );
	}
	
	const FSTree_Premapped::Mapping sys_mac_Mappings[] =
	{
		{ "proc",    &New_FSTree_sys_mac_proc    },
		{ "tempmem", &New_FSTree_sys_mac_tempmem },
		{ "thng",    &New_FSTree_sys_mac_thng    },
		//{ "window", &Basic_Factory< FSTree_sys_mac_window > },
		
	#if defined( __MACOS__ )  &&  !TARGET_API_MAC_CARBON
		
		{ "crm",  &Premapped_Factory< sys_mac_crm_Mappings  > },
		{ "crsr", &Premapped_Factory< sys_mac_crsr_Mappings > },
		
		{ "adb",   &New_FSTree_sys_mac_adb   },
		{ "drive", &New_FSTree_sys_mac_drive },
		{ "unit",  &New_FSTree_sys_mac_unit  },
		
		{ "xpram", &Generated_Factory< sys_mac_xpram > },
		
	#endif
		
		{ "desktop", &Premapped_Factory< sys_mac_desktop_Mappings > },
		{ "errata",  &Premapped_Factory< sys_mac_errata_Mappings  > },
		{ "event",   &Premapped_Factory< sys_mac_event_Mappings   > },
		{ "gdev",    &Premapped_Factory< sys_mac_gdev_Mappings    > },
		{ "machine", &Premapped_Factory< sys_mac_machine_Mappings > },
		{ "time",    &Premapped_Factory< sys_mac_time_Mappings    > },
		{ "user",    &Premapped_Factory< sys_mac_user_Mappings    > },
		{ "vol",     &Premapped_Factory< sys_mac_vol_Mappings     > },
		
		{ "gestalt", &New_FSTree_sys_mac_gestalt },
		
		{ "name", &Property_Factory< sys_mac_name > },
		{ "keys", &Generated_Factory< sys_mac_keys > },
		
		{ "rom", &New_FSTree_sys_mac_rom },
		
		{ NULL, NULL }
		
	};
	
}

