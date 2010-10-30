/*
	GetAppFolder.hh
	---------------
	
	Copyright 2009, Joshua Juran
*/

#ifndef GENIE_UTILITIES_GETAPPFOLDER_HH
#define GENIE_UTILITIES_GETAPPFOLDER_HH

// Nitrogen
#ifndef MAC_FILES_TYPES_FSDIRSPEC_HH
#include "Mac/Files/Types/FSDirSpec.hh"
#endif


namespace Genie
{
	
	Mac::FSDirSpec GetAppFolder();
	
}

#endif

