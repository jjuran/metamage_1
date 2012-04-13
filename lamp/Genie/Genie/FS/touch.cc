/*
	touch.cc
	--------
*/

#include "Genie/FS/touch.hh"

// POSIX
#include <time.h>

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/FS/utime.hh"


#ifndef UTIME_NOW
#define UTIME_NOW   ((1 << 30) - 1)
#endif


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	void touch( const FSTree* it )
	{
		const node_method_set* methods = it->methods();
		
		if ( methods  &&  methods->touch )
		{
			methods->touch( it );
		}
		else
		{
			const struct timespec times[2] = { { 0, UTIME_NOW }, { 0, UTIME_NOW } };
			
			utime( it, times );
		}
	}
	
}

