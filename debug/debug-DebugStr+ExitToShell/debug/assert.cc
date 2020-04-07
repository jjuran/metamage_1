/*	===============
 *	debug/assert.cc
 *	===============
 */

#include "debug/assert.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

// mac-sys-utils
#include "mac_sys/exit_to_shell.hh"


namespace debug
{
	
	void handle_failed_assertion( const char*  text,
	                              const char*  func,
	                              const char*  file,
	                              unsigned     line,
	                              bool         fatal )
	{
		::DebugStr( "\p" "Assertion failure" );
		
		if ( fatal )
		{
			mac::sys::exit_to_shell();
		}
	}
	
}
