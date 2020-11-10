/*
	container.cc
	------------
*/

#include "vxo/container.hh"

// Standard C
#include <string.h>

// debug
#include "debug/assert.hh"

// plus
#include "plus/datum_alloc.hh"
#include "plus/extent.hh"


namespace vxo
{

bool Container::test( const Box& box )
{
	return box.has_pointer()  &&  box.subtype_byte() >= Box_container;
}

Container::Container( box_type type, size_t n_items )
{
	u.str.pointer  = NULL;
	u.str.length   = 0;
	u.str.capacity = 0;
	
	set_subtype_byte( type );
	set_control_byte( Box_pointer );
	
	if ( n_items > 0 )
	{
		const size_t n_bytes = n_items * sizeof (Box);
		
		// Passing a destructor (even if NULL) clears the buffer.
		
		if ( char* alloc = plus::extent_alloc( n_bytes, NULL ) )
		{
			u.str.pointer  = alloc;
			u.str.capacity = n_items;
			
			set_control_byte( Box_shared );
		}
	}
}

Box* Container::expand_by( size_t n )
{
	const size_t original_length = u.str.length;
	
	ASSERT( is< Container >() );
	
	ASSERT( u.str.length <= u.str.capacity );
	
	const size_t new_length = u.str.length + n;
	
	if ( new_length > u.str.capacity )
	{
		u.str.length   *= sizeof (Box);
		u.str.capacity *= sizeof (Box);
		
		size_t new_capacity = u.str.capacity * 2;
		
		if ( new_capacity < new_length * sizeof (Box) )
		{
			new_capacity = new_length * sizeof (Box);
		}
		
		char* alloc;
		
		if ( u.str.pointer == NULL )
		{
			alloc = plus::extent_alloc( new_capacity );
			
			u.str.pointer  = alloc;
			u.str.capacity = new_capacity;
			
			set_control_byte( Box_shared );
		}
		else
		{
			plus::datum_storage& storage = *(plus::datum_storage*) this;
			
			alloc = extend_capacity( storage, new_capacity );
		}
		
		size_t skipped = u.str.length;
		size_t cleared = new_capacity - skipped;
		
		memset( alloc + skipped, '\0', cleared );
		
		u.str.capacity /= sizeof (Box);
	}
	
	u.str.length = new_length;
	
	return begin() + original_length;
}

}
