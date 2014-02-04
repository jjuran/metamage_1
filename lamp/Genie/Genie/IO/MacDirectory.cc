/*
	MacDirectory.cc
	---------------
*/

#include "Genie/IO/MacDirectory.hh"

// vfs
#include "vfs/node.hh"

// Genie
#include "Genie/FS/FSSpec.hh"


namespace Genie
{
	
	vfs::node_ptr MacDirHandle::GetFile()
	{
		return FSTreeFromFSDirSpec( its_dir_spec );
	}
	
}

