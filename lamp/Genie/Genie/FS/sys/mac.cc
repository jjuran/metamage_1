/*
	Genie/FS/sys/mac.cc
	-------------------
*/

#include "Genie/FS/sys/mac.hh"

// vfs
#include "vfs/nodes/fixed_dir.hh"

// relix-kernel
#include "relix/config/color.hh"
#include "relix/config/mini.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/FSTree_Generated.hh"
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/utf8_text_property.hh"
#include "Genie/FS/sys/mac/desktop.hh"
#include "Genie/FS/sys/mac/errata.hh"
#include "Genie/FS/sys/mac/event.hh"
#include "Genie/FS/sys/mac/gdev.hh"
#include "Genie/FS/sys/mac/gestalt.hh"
#include "Genie/FS/sys/mac/keys.hh"
#include "Genie/FS/sys/mac/machine.hh"
#include "Genie/FS/sys/mac/name.hh"
#include "Genie/FS/sys/mac/proc.hh"
#include "Genie/FS/sys/mac/rom.hh"
#include "Genie/FS/sys/mac/soundin.hh"
#include "Genie/FS/sys/mac/tempmem.hh"
#include "Genie/FS/sys/mac/thng.hh"
#include "Genie/FS/sys/mac/thread.hh"
#if defined( __MACOS__ )  &&  !TARGET_API_MAC_CARBON
#include "Genie/FS/sys/mac/adb.hh"
#include "Genie/FS/sys/mac/crm.hh"
#include "Genie/FS/sys/mac/crsr.hh"
#include "Genie/FS/sys/mac/drive.hh"
#include "Genie/FS/sys/mac/unit.hh"
#include "Genie/FS/sys/mac/xpram.hh"
#endif
#include "Genie/FS/sys/mac/time.hh"
#include "Genie/FS/sys/mac/user.hh"
#include "Genie/FS/sys/mac/vol.hh"


#ifndef CONFIG_SYS_MAC_ADB
#define CONFIG_SYS_MAC_ADB  (!CONFIG_MINI)
#endif

#ifndef CONFIG_SYS_MAC_CRM
#define CONFIG_SYS_MAC_CRM  (!CONFIG_MINI)
#endif

#ifndef CONFIG_SYS_MAC_GDEV
#define CONFIG_SYS_MAC_GDEV  CONFIG_COLOR
#endif

#ifndef CONFIG_SYS_MAC_SOUNDIN
#define CONFIG_SYS_MAC_SOUNDIN  (!CONFIG_MINI)
#endif

namespace Genie
{
	
	#define PREMAPPED( map )  &vfs::fixed_dir_factory, (const void*) map
	
	#define PROPERTY( prop )  &new_property, &property_params_factory< prop >::value
	
	const vfs::fixed_mapping sys_mac_Mappings[] =
	{
		{ "proc",    &New_FSTree_sys_mac_proc    },
		
	#if CONFIG_SYS_MAC_SOUNDIN
		
		{ "soundin", &New_FSTree_sys_mac_soundin },
		
	#endif
		
		{ "tempmem", &New_FSTree_sys_mac_tempmem },
		{ "thng",    &New_FSTree_sys_mac_thng    },
		
	#if defined( __MACOS__ )  &&  !TARGET_API_MAC_CARBON
		
	#if CONFIG_SYS_MAC_CRM
		
		{ "crm",  PREMAPPED( sys_mac_crm_Mappings  ) },
		
	#endif
		
		{ "crsr", PREMAPPED( sys_mac_crsr_Mappings ) },
		
	#if CONFIG_SYS_MAC_ADB
		
		{ "adb",   &New_FSTree_sys_mac_adb   },
		
	#endif
		
		{ "drive", &New_FSTree_sys_mac_drive },
		{ "unit",  &New_FSTree_sys_mac_unit  },
		
		{ "xpram", &new_generated, (void*) &sys_mac_xpram::Read },
		
	#endif
		
		{ "desktop", PREMAPPED( sys_mac_desktop_Mappings ) },
		{ "errata",  PREMAPPED( sys_mac_errata_Mappings  ) },
		{ "event",   PREMAPPED( sys_mac_event_Mappings   ) },
		
	#if CONFIG_SYS_MAC_GDEV
		
		{ "gdev",    PREMAPPED( sys_mac_gdev_Mappings    ) },
		
	#endif
		
		{ "machine", PREMAPPED( sys_mac_machine_Mappings ) },
		{ "thread",  PREMAPPED( sys_mac_thread_Mappings  ) },
		{ "time",    PREMAPPED( sys_mac_time_Mappings    ) },
		{ "user",    PREMAPPED( sys_mac_user_Mappings    ) },
		{ "vol",     PREMAPPED( sys_mac_vol_Mappings     ) },
		
		{ "gestalt", &New_FSTree_sys_mac_gestalt },
		
		{ ".mac-name", PROPERTY(                     sys_mac_name   ) },
		{      "name", PROPERTY( utf8_text_property< sys_mac_name > ) },
		
		{ "keys", &new_generated, (void*) &sys_mac_keys::Read },
		
		{ "rom", &New_FSTree_sys_mac_rom },
		
		{ NULL, NULL }
		
	};
	
}

