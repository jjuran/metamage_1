/*	====================
 *	FreeTheMallocPool.cc
 *	====================
 */

// MSL
#include <pool_alloc.h>


#pragma exceptions off

extern "C" void* __global_destructor_chain;

extern "C" void FreeTheMallocPool();


void FreeTheMallocPool()
{
	__global_destructor_chain = 0L;
	
	__pool_free_all();
}

