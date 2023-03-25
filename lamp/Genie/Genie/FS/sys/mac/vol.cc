/*
	Genie/FS/sys/mac/vol.cc
	-----------------------
*/

#include "Genie/FS/sys/mac/vol.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// mac-file-utils
#include "mac_file/boot_volume.hh"

// gear
#include "gear/inscribe_decimal.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/node/types/symbolic_link.hh"

// Genie
#include "Genie/FS/sys/mac/vol/list.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	static vfs::node_ptr new_volume_link( const vfs::node*     parent,
	                                      const plus::string&  name,
	                                      const void*          args )
	{
		typedef short (*Function)();
		
		Function f = (Function) args;
		
		const SInt16 vRefNum = f();
		
		if ( vRefNum == 0 )
		{
			p7::throw_errno( ENOENT );
		}
		
		plus::var_string result = "list/";
		
		result += gear::inscribe_decimal( -vRefNum );
		
		return vfs::new_symbolic_link( parent, name, result );
		
	}
	
	const vfs::fixed_mapping sys_mac_vol_Mappings[] =
	{
		{ "list", &New_FSTree_sys_mac_vol },
		
		{ "boot", &new_volume_link, (void*) &mac::file::boot_volume },
		
		{ NULL, NULL }
		
	};
	
}
