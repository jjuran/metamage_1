/*
	MacDirectory.cc
	---------------
*/

#include "Genie/IO/MacDirectory.hh"

// Genie
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	FSTreePtr MacDirHandle::GetFile()
	{
		return FSTreeFromFSDirSpec( its_dir_spec );
	}
	
}

