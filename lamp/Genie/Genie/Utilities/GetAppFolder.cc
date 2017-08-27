/*
	GetAppFolder.cc
	---------------
*/

#include "Genie/Utilities/GetAppFolder.hh"

// mac-file-utils
#include "mac_file/parent_directory.hh"
#include "mac_file/program_file.hh"


namespace Genie
{
	
	using mac::file::parent_directory;
	using mac::file::program_file;
	
	
	mac::types::VRefNum_DirID GetAppFolder()
	{
		return parent_directory( program_file() );
	}
	
}
