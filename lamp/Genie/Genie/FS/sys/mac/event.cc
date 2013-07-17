/*
	Genie/FS/sys/mac/event.cc
	-------------------------
*/

#include "Genie/FS/sys/mac/event.hh"

// vfs
#include "vfs/node/types/fixed_dir.hh"

// Genie
#include "Genie/FS/sys/mac/event/post.hh"


namespace Genie
{
	
	#define PREMAPPED( map )  &vfs::fixed_dir_factory, (const void*) map
	
	const vfs::fixed_mapping sys_mac_event_Mappings[] =
	{
		{ "post", PREMAPPED( sys_mac_event_post_Mappings ) },
		
		{ NULL, NULL }
	};
	
}

