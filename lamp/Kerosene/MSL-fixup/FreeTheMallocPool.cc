/*	====================
 *	FreeTheMallocPool.cc
 *	====================
 */


#include "FreeTheMallocPool.h"

// MSL
#include <pool_alloc.h>


#pragma exceptions off

extern "C" void __destroy_global_chain();

extern "C" void* __global_destructor_chain;


void FreeTheMallocPool( short destroying_globals )
{
	if ( destroying_globals )
	{
		__destroy_global_chain();
	}
	else
	{
		__global_destructor_chain = 0L;
	}
	
	__pool_free_all();
}

