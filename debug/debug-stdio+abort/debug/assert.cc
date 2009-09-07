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
	                              unsigned     line )
	{
		fprintf( stderr,
		         "ASSERT( %s ) in %s; %s, line %d\n",
		         text,
		         func,
		         file,
		         line );
		
		abort();
	}
	
}

