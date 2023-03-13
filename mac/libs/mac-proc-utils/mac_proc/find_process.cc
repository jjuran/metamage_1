/*
	find_process.cc
	---------------
*/

#include "mac_proc/find_process.hh"

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __PROCESSES__
#include <Processes.h>
#endif

// mac-file-utils
#include "mac_file/compare.hh"


namespace mac  {
namespace proc {

OSErr find_process( ProcessSerialNumber& psn, OSType signature )
{
	psn.highLongOfPSN = 0;
	psn.lowLongOfPSN  = 0;
	
	OSErr err;
	ProcessInfoRec processInfo;
	
	processInfo.processInfoLength = sizeof processInfo;
	processInfo.processName       = NULL;
#if __LP64__
	processInfo.processAppRef     = NULL;
#else
	processInfo.processAppSpec    = NULL;
#endif
	
	while ( true )
	{
		err = GetNextProcess( &psn );
		
		if ( err != noErr )
		{
			break;
		}
		
		err = GetProcessInformation( &psn, &processInfo );
		
		if ( err == noErr  &&  processInfo.processSignature == signature )
		{
			return err;
		}
	}
	
	return err;
}

#ifndef __LP64__

OSErr find_process( ProcessSerialNumber& psn, const FSSpec& app_file )
{
	psn.highLongOfPSN = 0;
	psn.lowLongOfPSN  = 0;
	
	OSErr err;
	FSSpec file;
	ProcessInfoRec processInfo;
	
	processInfo.processInfoLength = sizeof processInfo;
	processInfo.processName       = NULL;
	processInfo.processAppSpec    = &file;
	
	while ( true )
	{
		err = GetNextProcess( &psn );
		
		if ( err != noErr )
		{
			break;
		}
		
		err = GetProcessInformation( &psn, &processInfo );
		
		if ( err == noErr  &&  mac::file::FSSpecs_are_same( file, app_file ) )
		{
			return err;
		}
	}
	
	return err;
}

#endif

}
}
