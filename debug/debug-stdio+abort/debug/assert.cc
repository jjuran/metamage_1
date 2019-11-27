/*	===============
 *	debug/assert.cc
 *	===============
 */

#include "debug/assert.hh"

// Standard C
#include <stdio.h>
#include <stdlib.h>


namespace debug
{
	
	void handle_failed_assertion( const char*  text,
	                              const char*  func,
	                              const char*  file,
	                              unsigned     line,
	                              bool         fatal )
	{
		fprintf( stderr,
		         "%s( %s ) in %s; %s, line %d\n",
		         fatal ? "ASSERT" : "EXPECT",
		         text,
		         func,
		         file,
		         line );
		
		if ( fatal )
		{
			abort();
		}
	}
	
}
