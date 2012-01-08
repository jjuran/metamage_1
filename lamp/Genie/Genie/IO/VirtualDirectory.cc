/*
	VirtualDirectory.cc
	-------------------
*/

#include "Genie/IO/VirtualDirectory.hh"

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	VirtualDirHandle::VirtualDirHandle( const FSTreePtr& tree )
	:
		its_dir( tree )
	{
	}
	
	VirtualDirHandle::~VirtualDirHandle()
	{
	}
	
	FSTreePtr VirtualDirHandle::GetFile()
	{
		return its_dir;
	}
	
}

