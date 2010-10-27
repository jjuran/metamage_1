/*
	Genie/FS/sys/mac/event.cc
	-------------------------
*/

#include "Genie/FS/sys/mac/event.hh"

// Genie
#include "Genie/FS/premapped.hh"
#include "Genie/FS/sys/mac/event/post.hh"


namespace Genie
{
	
	#define PREMAPPED( map )  &premapped_factory, (const void*) map
	
	const FSTree_Premapped::Mapping sys_mac_event_Mappings[] =
	{
		{ "post", PREMAPPED( sys_mac_event_post_Mappings ) },
		
		{ NULL, NULL }
	};
	
}

