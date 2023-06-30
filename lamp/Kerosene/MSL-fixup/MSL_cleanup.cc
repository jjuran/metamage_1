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
	
#if __heap_version == 1
	
	__pool_free_all( &__malloc_pool );
	
#endif
	
#if __heap_version == 2
	
	__pool_free_all();
	
#endif
}
