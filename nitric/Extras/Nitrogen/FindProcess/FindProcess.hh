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
#include "Nitrogen/MacTypes.hh"


namespace NitrogenExtras
{
	
	ProcessSerialNumber FindProcess( Nitrogen::OSType signature );
	
	ProcessSerialNumber FindProcess( const FSSpec& appFile );
	
}

#endif

