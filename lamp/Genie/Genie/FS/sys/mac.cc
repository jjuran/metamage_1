/*
	Genie/FS/sys/mac.cc
	-------------------
*/

#include "Genie/FS/sys/mac.hh"

// mac-sys-utils
#include "mac_sys/xpram.hh"

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

// Genie
#include "Genie/FS/sys/mac/gestalt.hh"
#include "Genie/FS/sys/mac/user.hh"
#include "Genie/FS/sys/mac/vol.hh"


namespace Genie
{
	
	static
	plus::string read_xpram( const vfs::node* parent, const plus::string& name )
	{
		const UInt16 length = 256;
		const UInt16 offset = 0;
		
		plus::string xpram;
		
		char* p = xpram.reset( length );
		
	#if ! TARGET_API_MAC_CARBON
		
		mac::sys::ReadXPRam( p, length, offset );
		
	#endif
		
		return xpram;
	}
	
	#define PREMAPPED( map )  &vfs::fixed_dir_factory, (const void*) map
	
	#define PROPERTY( prop )  &vfs::new_property, &prop##_params
	
	const vfs::fixed_mapping sys_mac_Mappings[] =
	{
		{ "tempmem", &vfs::new_sys_mac_tempmem },
		
	#if defined( __MACOS__ )  &&  !TARGET_API_MAC_CARBON
		
		{ "xpram", &vfs::new_generated, (void*) &read_xpram },
		
	#endif
		
		{ "user",    PREMAPPED( sys_mac_user_Mappings    ) },
		{ "vol",     PREMAPPED( sys_mac_vol_Mappings     ) },
		
		{ "gestalt", &New_FSTree_sys_mac_gestalt },
		
		{ "rom",     &vfs::new_sys_mac_rom     },
		{ "bootrom", &vfs::new_sys_mac_bootrom },
		
		{ NULL, NULL }
		
	};
	
}
