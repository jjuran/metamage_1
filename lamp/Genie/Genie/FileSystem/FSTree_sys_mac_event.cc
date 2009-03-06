/*	=======================
 *	FSTree_sys_mac_event.cc
 *	=======================
 */

#include "Genie/FileSystem/FSTree_sys_mac_event.hh"

// Genie
#include "Genie/FileSystem/FSTree_event_post.hh"


namespace Genie
{
	
	const FSTree_Premapped::Mapping sys_mac_event_Mappings[] =
	{
		{ "post", &Premapped_Factory< sys_mac_event_post_Mappings > },
		
		{ NULL, NULL }
	};
	
}

