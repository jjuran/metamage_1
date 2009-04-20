/*	====================
 *	FreeTheMallocPool.cc
 *	====================
 */


#include "FreeTheMallocPool.h"

// MSL
#include <pool_alloc.h>


#pragma exceptions off

extern "C" void* __global_destructor_chain;


void FreeTheMallocPool()
{
	__global_destructor_chain = 0L;
	
	__pool_free_all();
}

