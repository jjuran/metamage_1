/*	===========
 *	Processes.h
 *	===========
 */

#ifndef UTILITIES_PROCESSES_H
#define UTILITIES_PROCESSES_H

// Nitrogen
#include "Nitrogen/Processes.h"


namespace NitrogenExtras
{
	
	ProcessSerialNumber FindProcess( Nitrogen::OSType signature );
	
	ProcessSerialNumber LaunchApplication( Nitrogen::OSType signature );
	
}

#endif

