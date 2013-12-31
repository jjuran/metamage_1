/*
	GetAppFolder.cc
	---------------
	
	Copyright 2009, Joshua Juran
*/

#include "Genie/Utilities/GetAppFolder.hh"

// mac-sys-utils
#include "mac_sys/get_process_file.hh"

// Io: MacFiles
#include "MacFiles/Classic.hh"


namespace Genie
{
	
	Mac::FSDirSpec GetAppFolder()
	{
		const FSSpec appFile = mac::sys::get_process_file();
		
		return io::get_preceding_directory( appFile );
	}
	
}

