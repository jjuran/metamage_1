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
#include "mac_sys/has/ProcessManager.hh"


namespace mac {
namespace sys {
	
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
	
	struct ProcessSerialNumber
	{
		::ProcessSerialNumber psn;
		
		ProcessSerialNumber()
		{
			// Don't invoke the _OSDispatch trap if it's not available.
			// (Non-68K Macs always have the Process Manager.)
			
			if ( ! TARGET_CPU_68K  ||  has_ProcessManager() )
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
