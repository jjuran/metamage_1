/*
	GetAppFile.cc
	-------------
	
	Copyright 2009, Joshua Juran
*/

#include "Genie/Utilities/GetAppFile.hh"

// Nitrogen
#include "Nitrogen/Files.hh"
#include "Nitrogen/Processes.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	FSSpec GetAppFile()
	{
		FSSpec appFile;
		
		ProcessInfoRec info = NN::Make< ProcessInfoRec >( &appFile );
		
		N::GetProcessInformation( N::CurrentProcess(), info );
		
		return appFile;
	}
	
}

