/*
	buffers.cc
	----------
*/

#include "buffers.hh"

// Standard C
#include <stdlib.h>

// libsndtrack
#include "exceptions.hh"


void* alloc_buffer( unsigned minimum_length )
{
	void* alloc = malloc( minimum_length );
	
	if ( alloc == NULL )
	{
		take_exception( out_of_memory );
	}
	
	return alloc;
}

void free_buffer( void* buffer )
{
	free( buffer );
}
