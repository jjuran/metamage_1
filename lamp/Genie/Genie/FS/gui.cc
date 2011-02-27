/*
	Genie/FS/gui.cc
	---------------
*/

#include "Genie/FS/gui.hh"

// Genie
#include "Genie/FS/FSTree_new.hh"
#include "Genie/FS/premapped.hh"


namespace Genie
{
	
	#define PREMAPPED( map )  &premapped_factory, (const void*) map
	
	const FSTree_Premapped::Mapping gui_Mappings[] =
	{
		{ "new", PREMAPPED( new_Mappings ) },
		
		{ NULL, NULL }
	};
	
}

