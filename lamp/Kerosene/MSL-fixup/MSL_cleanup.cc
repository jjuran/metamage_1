/*
	MSL_cleanup.cc
	--------------
*/

// MSL
#include <pool_alloc.h>


#pragma exceptions off

extern "C" void* __global_destructor_chain;

extern "C" void _MSL_cleanup();


void _MSL_cleanup()
{
	__global_destructor_chain = 0L;
	
	__pool_free_all();
}

