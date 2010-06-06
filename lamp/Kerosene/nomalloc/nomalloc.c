#include <stdlib.h>

extern void* __global_destructor_chain;

extern void _MSL_cleanup();


void _MSL_cleanup()
{
	__global_destructor_chain = 0L;
}


extern void* malloc_is_unimplemented();

void* malloc( size_t )
{
	return malloc_is_unimplemented();
}

void* calloc( size_t, size_t )
{
	return malloc_is_unimplemented();
}

void* realloc( void*, size_t )
{
	return malloc_is_unimplemented();
}

void free( void* )
{
	malloc_is_unimplemented();
}

