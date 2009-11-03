/*
	GetAppFolder.cc
	---------------
	
	Copyright 2009, Joshua Juran
*/

#include "Genie/Utilities/GetAppFolder.hh"

// Io: MacFiles
#include "MacFiles.hh"

// Genie
#include "Genie/Utilities/GetAppFile.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	N::FSDirSpec GetAppFolder()
	{
		const FSSpec appFile = GetAppFile();
		
		return io::get_preceding_directory( appFile );
	}
	
}

