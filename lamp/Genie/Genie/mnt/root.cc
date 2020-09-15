/*
	root.cc
	-------
*/

#include "Genie/mnt/root.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/node/types/fixed_dir.hh"

// Genie
#include "Genie/FS/gui.hh"


namespace Genie {

#define PREMAPPED( map )  &vfs::fixed_dir_factory, (const void*) map

static const vfs::fixed_mapping root_mappings[] =
{
	{ "gui", PREMAPPED( gui_Mappings ) },
	
	{ NULL, NULL }
};

const vfs::node& freemount_root()
{
	static vfs::node_ptr root = fixed_dir( NULL,
	                                       plus::string::null,
	                                       root_mappings );
	
	return *root;
}

}  // namespace Genie
