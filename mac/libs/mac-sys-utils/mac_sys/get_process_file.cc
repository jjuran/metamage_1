/*
	get_process_file.cc
	-------------------
*/

#include "mac_sys/get_process_file.hh"

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __PROCESSES__
#include <Processes.h>
#endif


namespace mac {
namespace sys {
	
	mac::types::FSSpec get_process_file()
	{
		::FSSpec file;
		
		const ProcessSerialNumber psn = { 0, kCurrentProcess };
		
		ProcessInfoRec processInfo = { 0 };
		
		processInfo.processInfoLength = sizeof processInfo;
		processInfo.processName       = NULL;
		processInfo.processAppSpec    = &file;
		
		OSErr err = ::GetProcessInformation( &psn, &processInfo );
		
		if ( err != noErr )
		{
			// This shouldn't happen
			::ExitToShell();
		}
		
		return (const mac::types::FSSpec&) file;
	}
	
}
}

