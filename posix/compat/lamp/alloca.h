/*
	alloca.h
	--------
*/

#ifndef LAMP_ALLOCA_H
#define LAMP_ALLOCA_H

// Standard C
#include <signal.h>
#include <stdlib.h>

// Lamp
#include "lamp/stack.h"

#ifdef __cplusplus
extern "C" {
#endif


inline size_t _lamp_alloca_space()
{
	const size_t margin = 5 * 1024;  // leave 5K available
	
	const size_t space = _lamp_stack_space();
	
	return space > margin ? space - margin : 0;
}

inline int is_excessive_for_alloca( size_t size )
{
#ifdef __LAMP__
	
	return size > _lamp_alloca_space();
	
#endif
	
	return false;
}


inline void* safe_alloca( size_t length )
{
	if ( is_excessive_for_alloca( length ) )
	{
		return NULL;
	}
	
	return alloca( length );
}

inline void* checked_alloca( size_t length )
{
	if ( is_excessive_for_alloca( length ) )
	{
		abort();
	}
	
	return alloca( length );
}

inline void* signalling_alloca( size_t length )
{
#ifdef SIGSTKFLT
	
	if ( is_excessive_for_alloca( length ) )
	{
		raise( SIGSTKFLT );
	}
	
#endif
	
	return alloca( length );
}

#ifdef __cplusplus
}
#endif

#endif

