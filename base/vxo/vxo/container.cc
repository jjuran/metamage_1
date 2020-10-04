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

Box* Container::append( const Box& item )
{
	ASSERT( is< Container >() );
	
	ASSERT( u.str.length <= u.str.capacity );
	
	if ( u.str.length == u.str.capacity )
	{
		u.str.length   *= sizeof (Box);
		u.str.capacity *= sizeof (Box);
		
		size_t new_capacity = u.str.capacity * 2;
		
		char* alloc;
		
		if ( new_capacity == 0 )
		{
			new_capacity = 8 * sizeof (Box);
			
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
		
		u.str.length   /= sizeof (Box);
		u.str.capacity /= sizeof (Box);
	}
	
	Box* it = begin() + u.str.length++;
	
	*it = item;
	
	return it;
}

}
