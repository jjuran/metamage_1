/*
	Root_Overlay.cc
	---------------
	
	Copyright 2010, Joshua Juran
*/

#include "Genie/FS/Root_Overlay.hh"

// Genie
#include "Genie/FS/app.hh"
#include "Genie/FS/FSTree_Dev.hh"
#include "Genie/FS/FSTree_Proc.hh"
#include "Genie/FS/FSTree_new.hh"
#include "Genie/FS/premapped.hh"
#include "Genie/FS/sys.hh"
#include "Genie/FS/Users.hh"
#include "Genie/FS/Volumes.hh"


namespace Genie
{
	
	#define PREMAPPED( map )  &premapped_factory, (const void*) map
	
	const FSTree_Premapped::Mapping Root_Overlay_Mappings[] =
	{
		{ "Users",   &New_FSTree_Users   },
		{ "Volumes", &New_FSTree_Volumes },
		
		{ "app",  &New_FSTree_app  },
		{ "proc", &New_FSTree_proc },
		
		{ "dev", PREMAPPED( dev_Mappings ) },
		{ "new", PREMAPPED( new_Mappings ) },
		{ "sys", PREMAPPED( sys_Mappings ) },
		
		{ NULL, NULL }
	};
	
}

