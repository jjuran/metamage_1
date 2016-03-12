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

// mac-sys-utils
#include "mac_sys/gestalt.hh"


namespace mac {
namespace sys {
	
	static inline
	bool gestalt_LaunchCanReturn()
	{
		// Non-68K Macs always have the Process Manager.
		
		if ( ! TARGET_CPU_68K )
		{
			return true;
		}
		
		enum
		{
			gestaltOSAttr          = 'os  ',
			
			gestaltLaunchCanReturn = 1,
		};
		
		return gestalt_bit_set( gestaltOSAttr, gestaltLaunchCanReturn );
	}
	
	/*
		We can call Process Manager routines in System 7, or in System 6
		with MultiFinder.  Presumably we could check for the _OSDispatch
		trap, but a Gestalt() call seems a little cleaner and in any case
		is fewer trap calls.
		
		The documentation for gestaltLaunchCanReturn in _Inside Macintosh:
		Processes_ doesn't refer to functions like GetCurrentProcess() or
		to _OSDispatch (the underlying trap), but it does suggest that it's
		a reliable indicator of a multi-process-capable system.
	*/
	
	const bool multiproc_available = gestalt_LaunchCanReturn();
	
	struct ProcessSerialNumber
	{
		::ProcessSerialNumber psn;
		
		ProcessSerialNumber()
		{
			// Don't invoke the _OSDispatch trap if it's not available.
			
			if ( multiproc_available )
			{
				::GetCurrentProcess( &psn );
			}
		}
	};
	
	static ProcessSerialNumber gProcessSerialNumber;
	
	const ::ProcessSerialNumber& current_process()
	{
		return gProcessSerialNumber.psn;
	}
	
}
}
