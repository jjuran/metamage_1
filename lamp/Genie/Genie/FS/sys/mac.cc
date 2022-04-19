/*
	Genie/FS/sys/mac.cc
	-------------------
*/

#include "Genie/FS/sys/mac.hh"

// mac-config
#include "mac_config/adb.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/property.hh"
#include "vfs/node/types/fixed_dir.hh"
#include "vfs/node/types/generated_file.hh"
#include "vfs/node/types/property_file.hh"

// MacVFS
#include "MacVFS/file/sys_mac_bootrom.hh"
#include "MacVFS/file/sys_mac_rom.hh"
#include "MacVFS/file/tempmem.hh"

// relix-kernel
#include "relix/config/color.hh"
#include "relix/config/mini.hh"

// Genie
#include "Genie/FS/sys/mac/desktop.hh"
#include "Genie/FS/sys/mac/event.hh"
#include "Genie/FS/sys/mac/gdev.hh"
#include "Genie/FS/sys/mac/gestalt.hh"
#include "Genie/FS/sys/mac/keys.hh"
#include "Genie/FS/sys/mac/name.hh"
#include "Genie/FS/sys/mac/proc.hh"
#include "Genie/FS/sys/mac/soundin.hh"
#include "Genie/FS/sys/mac/thng.hh"
#if defined( __MACOS__ )  &&  !TARGET_API_MAC_CARBON
#include "Genie/FS/sys/mac/adb.hh"
#include "Genie/FS/sys/mac/crm.hh"
#include "Genie/FS/sys/mac/crsr.hh"
#include "Genie/FS/sys/mac/drive.hh"
#include "Genie/FS/sys/mac/unit.hh"
#include "Genie/FS/sys/mac/xpram.hh"
#endif
#include "Genie/FS/sys/mac/user.hh"
#include "Genie/FS/sys/mac/vol.hh"


#ifndef CONFIG_SYS_MAC_ADB
#define CONFIG_SYS_MAC_ADB  (CONFIG_ADB  &&  ! CONFIG_MINI)
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
	
	#define PROPERTY( prop )  &vfs::new_property, &prop##_params
	
	const vfs::fixed_mapping sys_mac_Mappings[] =
	{
		{ "proc",    &New_FSTree_sys_mac_proc    },
		
	#if CONFIG_SYS_MAC_SOUNDIN
		
		{ "soundin", &New_FSTree_sys_mac_soundin },
		
	#endif
		
		{ "tempmem", &vfs::new_sys_mac_tempmem },
		{ "thng",    &New_FSTree_sys_mac_thng  },
		
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
		
		{ "xpram", &vfs::new_generated, (void*) &sys_mac_xpram::Read },
		
	#endif
		
		{ "desktop", PREMAPPED( sys_mac_desktop_Mappings ) },
		{ "event",   PREMAPPED( sys_mac_event_Mappings   ) },
		
	#if CONFIG_SYS_MAC_GDEV
		
		{ "gdev",    PREMAPPED( sys_mac_gdev_Mappings    ) },
		
	#endif
		
		{ "user",    PREMAPPED( sys_mac_user_Mappings    ) },
		{ "vol",     PREMAPPED( sys_mac_vol_Mappings     ) },
		
		{ "gestalt", &New_FSTree_sys_mac_gestalt },
		
		{ ".mac-name", PROPERTY( sys_mac_macname ) },
		{      "name", PROPERTY( sys_mac_name    ) },
		
		{ "keys", &vfs::new_generated, (void*) &sys_mac_keys::Read },
		
		{ "rom",     &vfs::new_sys_mac_rom     },
		{ "bootrom", &vfs::new_sys_mac_bootrom },
		
		{ NULL, NULL }
		
	};
	
}
