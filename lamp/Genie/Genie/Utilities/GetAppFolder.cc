/*
	GetAppFolder.cc
	---------------
	
	Copyright 2009, Joshua Juran
*/

#include "Genie/Utilities/GetAppFolder.hh"

// mac-sys-utils
#include "mac_sys/get_process_file.hh"

// mac-file-utils
#include "mac_file/parent_directory.hh"


namespace Genie
{
	
	using mac::file::parent_directory;
	
	
	mac::types::VRefNum_DirID GetAppFolder()
	{
		return parent_directory( mac::sys::get_process_file() );
	}
	
}
