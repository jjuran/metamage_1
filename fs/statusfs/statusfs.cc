/*
	statusfs.cc
	-----------
*/

#include "statusfs.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/functions/gethostname.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/property.hh"
#include "vfs/node/types/property_file.hh"


namespace statusfs
{
	
	namespace p7 = poseven;
	
	
	struct hostname : vfs::readonly_property
	{
		static void get( plus::var_string& result, const vfs::node* that, bool binary )
		{
			result = p7::gethostname();
		}
	};
	
	const vfs::fixed_mapping mappings[] =
	{
		{ "hostname", &vfs::new_property, &vfs::property_params_factory< hostname >::value },
		
		{ NULL, NULL }
	};
	
}
