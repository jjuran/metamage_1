/*
	MacDirectory.cc
	---------------
*/

#include "Genie/IO/MacDirectory.hh"

// Genie
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/FSTree_Null.hh"


namespace Genie
{
	
	FSTreePtr MacDirHandle::GetFile()
	{
		try
		{
			return FSTreeFromFSDirSpec( its_dir_spec, it_is_on_server );
		}
		catch ( ... )
		{
			return FSNull();
		}
	}
	
}

