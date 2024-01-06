/*
	resource_handle.cc
	------------------
*/

#include "Genie/code/resource_handle.hh"

// mac-sys-utils
#include "mac_sys/get_address_mask.hh"


namespace Genie
{

#ifdef __MC68K__

unsigned long address_mask = mac::sys::get_address_mask();

#endif

}
