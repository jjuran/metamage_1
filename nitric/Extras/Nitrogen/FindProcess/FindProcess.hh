/*
	FindProcess.hh
	--------------
	
	Copyright 2009, Joshua Juran
*/

#ifndef FINDPROCESS_HH
#define FINDPROCESS_HH

// Mac OS
#ifndef __PROCESSES__
#include <Processes.h>
#endif

// Nitrogen
#ifndef MAC_FILES_TYPES_FSCREATOR_HH
#include "Mac/Files/Types/FSCreator.hh"
#endif


namespace NitrogenExtras
{
	
	ProcessSerialNumber FindProcess( Mac::FSCreator signature );
	
	ProcessSerialNumber FindProcess( const FSSpec& appFile );
	
}

#endif

