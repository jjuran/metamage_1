/*
	touch.cc
	--------
*/

#include "vfs/primitives/touch.hh"

// POSIX
#include <time.h>

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/primitives/utime.hh"


#ifndef UTIME_NOW
#define UTIME_NOW   ((1 << 30) - 1)
#endif


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	void touch( const node& that )
	{
		const node_method_set* methods = that.methods();
		
		if ( methods  &&  methods->touch )
		{
			methods->touch( &that );
		}
		else
		{
			const struct timespec times[2] = { { 0, UTIME_NOW }, { 0, UTIME_NOW } };
			
			utime( &that, times );
		}
	}
	
}

