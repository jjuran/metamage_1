/*
	get_machine_name.cc
	-------------------
*/

#include "mac_sys/get_machine_name.hh"

// mac-sys-utils
#include "mac_sys/gestalt.hh"


namespace mac {
namespace sys {
	
	const unsigned char* get_machine_name()
	{
		enum { gestaltUserVisibleMachineName = 'mnam' };
		
		return (const unsigned char*) gestalt( gestaltUserVisibleMachineName );
	}
	
}
}
