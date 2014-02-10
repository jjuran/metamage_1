/*
	Genie/FS/sys/mac/machine/id.cc
	------------------------------
*/

#include "Genie/FS/sys/mac/machine/id.hh"

// mac-sys-utils
#include "mac_sys/gestalt.hh"

// plus
#include "plus/serialize.hh"
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"


#ifndef TARGET_API_MAC_CARBON
#define TARGET_API_MAC_CARBON  1
#endif


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	typedef unsigned long uint32_t;
	
	typedef plus::serialize_unsigned< uint32_t >::deconstruct deconstruct_id;
	
	
	const uint32_t gestaltMachineType = 'mach';
	
	
	static inline uint32_t GetMachineID()
	{
		const long id = mac::sys::gestalt( gestaltMachineType, -1 );
		
		if ( TARGET_API_MAC_CARBON  &&  id < 0 )
		{
			p7::throw_errno( ENOENT );
		}
		
		return id;
	}
	
	void sys_mac_machine_id::get( plus::var_string& result, const vfs::node* that, bool binary )
	{
		const uint32_t id = GetMachineID();
		
		deconstruct_id::apply( result, id, binary );
	}
	
}

