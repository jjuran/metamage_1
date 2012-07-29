/*
	Root_Overlay.cc
	---------------
	
	Copyright 2010, Joshua Juran
*/

#include "Genie/FS/Root_Overlay.hh"

// vfs
#include "vfs/nodes/fixed_dir.hh"

// Genie
#include "Genie/FS/FSTree_Dev.hh"
#include "Genie/FS/FSTree_Proc.hh"
#include "Genie/FS/gui.hh"
#include "Genie/FS/sys.hh"
#include "Genie/FS/Users.hh"
#include "Genie/FS/Volumes.hh"


namespace Genie
{
	
	#define PREMAPPED( map )  &vfs::fixed_dir_factory, (const void*) map
	
	const vfs::fixed_mapping Root_Overlay_Mappings[] =
	{
		{ "Users",   &New_FSTree_Users   },
		{ "Volumes", &New_FSTree_Volumes },
		
		{ "proc", &New_FSTree_proc },
		
		{ "dev", PREMAPPED( dev_Mappings ) },
		{ "gui", PREMAPPED( gui_Mappings ) },
		{ "sys", PREMAPPED( sys_Mappings ) },
		
		{ NULL, NULL }
	};
	
}

