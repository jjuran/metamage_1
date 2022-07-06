/*
	Genie/FS/sys.cc
	---------------
*/

#include "Genie/FS/sys.hh"

// mac-sys-utils
#include "mac_sys/beep.hh"

// mac-app-utils
#include "mac_app/commands.hh"

// vfs
#include "vfs/node/types/fixed_dir.hh"

// vfs-relix
#include "vfs/node/types/builtin.hh"

// Genie
#include "Genie/FS/sys/app.hh"
#include "Genie/FS/sys/mac.hh"
#include "Genie/FS/sys/type.hh"


namespace Genie
{
	
	static int true_main()
	{
		return 0;
	}
	
	static int false_main()
	{
		return 1;
	}
	
	static int beep_main()
	{
		mac::sys::beep();
		
		return 0;
	}
	
	static int quit_main()
	{
		mac::app::quit();
		
		return 0;
	}
	
	#define EXEC( main )  &vfs::builtin_factory, (const void*) &main
	
	extern const vfs::fixed_mapping sys_kernel_bin_Mappings[];
	
	const vfs::fixed_mapping sys_kernel_bin_Mappings[] =
	{
		{ "true",  EXEC( true_main  ) },
		{ "false", EXEC( false_main ) },
		
		{ "beep",  EXEC( beep_main  ) },
		{ "quit",  EXEC( quit_main  ) },
		
		{ NULL, NULL }
	};
	
	
	#define PREMAPPED( map )  &vfs::fixed_dir_factory, (const void*) map
	
	extern const vfs::fixed_mapping sys_kernel_Mappings[];
	
	const vfs::fixed_mapping sys_kernel_Mappings[] =
	{
		{ "bin",     PREMAPPED( sys_kernel_bin_Mappings ) },
		
		{ NULL, NULL }
	};
	
	const vfs::fixed_mapping sys_Mappings[] =
	{
		{ "app",    PREMAPPED( sys_app_Mappings    ) },
		{ "kernel", PREMAPPED( sys_kernel_Mappings ) },
		{ "mac",    PREMAPPED( sys_mac_Mappings    ) },
		{ "type",   PREMAPPED( sys_type_Mappings   ) },
		
		{ NULL, NULL }
	};
	
}
