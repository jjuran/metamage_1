/*
	Genie/FS/sys/mac/machine/name.cc
	--------------------------------
*/

#include "Genie/FS/sys/mac/machine/name.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/Utilities/GetMachineName.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	void sys_mac_machine_name::Read( plus::var_string& result, const FSTree* that, bool binary )
	{
		const unsigned char* name = GetMachineName();
		
		if ( !name )
		{
			p7::throw_errno( ENOENT );
		}
		
		result.assign( (const char*) &name[1], name[0] );
	}
	
}

