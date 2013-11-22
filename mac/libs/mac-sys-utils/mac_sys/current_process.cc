/*
	current_process.cc
	------------------
*/

#include "mac_sys/current_process.hh"

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
	
	struct ProcessSerialNumber
	{
		::ProcessSerialNumber psn;
		
		ProcessSerialNumber()
		{
			::GetCurrentProcess( &psn );
		}
	};
	
	static ProcessSerialNumber gProcessSerialNumber;
	
	const ::ProcessSerialNumber& current_process()
	{
		return gProcessSerialNumber.psn;
	}
	
}
}

