/*
	Genie/FS/sys/mac/machine/name.cc
	--------------------------------
*/

#include "Genie/FS/sys/mac/machine/name.hh"

// mac-sys-utils
#include "mac_sys/get_machine_name.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	void sys_mac_machine_name::get( plus::var_string& result, const vfs::node* that, bool binary )
	{
		const unsigned char* name = mac::sys::get_machine_name();
		
		if ( !name )
		{
			p7::throw_errno( ENOENT );
		}
		
		result.assign( (const char*) &name[1], name[0] );
	}
	
}

