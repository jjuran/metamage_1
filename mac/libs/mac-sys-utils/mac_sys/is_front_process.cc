/*
	is_front_process.cc
	-------------------
*/

#include "mac_sys/is_front_process.hh"

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __PROCESSES__
#include <Processes.h>
#endif

// mac-sys-utils
#include "mac_sys/current_process.hh"


namespace mac {
namespace sys {
	
	bool is_front_process( const ProcessSerialNumber& psn )
	{
		if ( TARGET_CPU_68K  &&  psn.lowLongOfPSN == 0 )
		{
			/*
				We're running in a single-process system, which means
				we're always the front process.  It's assumed that you're
				passing in mac::sys::current_process() -- in a multi-process
				environment, PSNs are never zero; in single-process, where
				else would you get a PSN than current_process()?
			*/
			
			return true;
		}
		
		ProcessSerialNumber front;
		
		if ( OSErr err = GetFrontProcess( &front ) )
		{
			/*
				paramErr      -50   Process serial number is invalid
				procNotFound  -600  No process in the foreground
				
				I have no idea how paramErr happens, but procNotFound
				(which I imagine could happen at interrupt time) clearly
				means the process is not in front.
			*/
			
			return false;
		}
		
		Boolean same;
		
		if ( OSErr err = SameProcess( &psn, &front, &same ) )
		{
			/*
				paramErr  -50  Process serial number is invalid
				
				This shouldn't happen, since psn and front are values returned
				from GetCurrentProcess() and GetFrontProcess() respectively.
			*/
			
			return false;
		}
		
		return same;
	}
	
}
}
