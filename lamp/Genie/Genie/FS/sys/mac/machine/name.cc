/*
	Genie/FS/sys/mac/machine/name.cc
	--------------------------------
*/

#include "Genie/FS/sys/mac/machine/name.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/Utilities/GetMachineName.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	plus::string sys_mac_machine_name::Read( const FSTree* that, bool binary )
	{
		const unsigned char* name = GetMachineName();
		
		if ( !name )
		{
			p7::throw_errno( ENOENT );
		}
		
		return plus::string( (const char*) &name[1], name[0] );
	}
	
}

