/*
	restack.c
	---------
*/

#include "relix/restack.h"

// Standard C
#include <stdlib.h>


#ifndef __LAMP__

void _relix_restack( unsigned param_length, const void* f, ... )
{
	// Not reached.  This shouldn't even be required to link, but it's gcc.
	
	abort();
}

#endif

