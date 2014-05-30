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
	
	MacDirHandle::MacDirHandle( const Mac::FSDirSpec& dir )
	:
		DirHandle( NULL ),
		its_dir_spec( dir )
	{
	}
	
	vfs::node_ptr MacDirHandle::GetFile()
	{
		return FSTreeFromFSDirSpec( its_dir_spec );
	}
	
}

