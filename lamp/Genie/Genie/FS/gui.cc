/*
	Genie/FS/gui.cc
	---------------
*/

#include "Genie/FS/gui.hh"

// Genie
#include "Genie/FS/premapped.hh"
#include "Genie/FS/gui/new.hh"
#include "Genie/FS/gui/port.hh"


namespace Genie
{
	
	#define PREMAPPED( map )  &premapped_factory, (const void*) map
	
	const FSTree_Premapped::Mapping gui_Mappings[] =
	{
		{ "new", PREMAPPED( new_Mappings ) },
		
		{ "port", &New_sys_port },
		
		{ NULL, NULL }
	};
	
}

