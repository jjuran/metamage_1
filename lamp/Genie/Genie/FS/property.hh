/*
	property.hh
	-----------
*/

#ifndef GENIE_FS_PROPERTY_HH
#define GENIE_FS_PROPERTY_HH

// vfs
#include "vfs/property.hh"


namespace Genie
{
	
	using vfs::property_get_hook;
	using vfs::property_set_hook;
	using vfs::readonly_property;
	using vfs::writeonly_property;
	using vfs::readwrite_property;
	
	using vfs::property_params;
	using vfs::property_params_factory;
	
}

#endif

