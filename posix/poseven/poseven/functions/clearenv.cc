/*
	clearenv.cc
	-----------
*/

#include "poseven/functions/clearenv.hh"

// POSIX
#include <stdlib.h>

// iota
#include "iota/dummy.hh"

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
#if !defined( __APPLE__ )  &&  !defined( __CYGWIN__ )
	
	void clearenv()
	{
		(void) throw_posix_result( ::clearenv() );
	}
	
#endif
	
}

