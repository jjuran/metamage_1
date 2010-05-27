/*
	GetAppFile.cc
	-------------
	
	Copyright 2009, Joshua Juran
*/

#include "Genie/Utilities/GetAppFile.hh"

// Nitrogen
#include "Nitrogen/Processes.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
	FSSpec GetAppFile()
	{
		FSSpec appFile;
		
		ProcessInfoRec info = n::make< ProcessInfoRec >( &appFile );
		
		N::GetProcessInformation( N::CurrentProcess(), info );
		
		return appFile;
	}
	
}

