/*	===============
 *	debug/assert.cc
 *	===============
 */

#include "debug/assert.hh"

// Mac OS
#include <MacTypes.h>
#include <Processes.h>


namespace debug
{
	
	void handle_failed_assertion( const char*  text,
	                              const char*  func,
	                              const char*  file,
	                              unsigned     line )
	{
		::DebugStr( "\p" "Assertion failure" );
		
		::ExitToShell();
	}
	
}

