/*
	Genie/FS/gui.cc
	---------------
*/

#include "Genie/FS/gui.hh"

// vfs
#include "vfs/node/types/fixed_dir.hh"

// Genie
#include "Genie/FS/gui/new.hh"
#include "Genie/FS/gui/port.hh"


namespace Genie
{
	
	#define PREMAPPED( map )  &vfs::fixed_dir_factory, (const void*) map
	
	const vfs::fixed_mapping gui_Mappings[] =
	{
		{ "new", PREMAPPED( new_Mappings ) },
		
		{ "port", &new_gui_port },
		
		{ NULL, NULL }
	};
	
}

