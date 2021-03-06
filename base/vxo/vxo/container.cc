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

Box* Container::insert_n( Item* loc, size_t n )
{
	ASSERT( loc >= begin() );
	ASSERT( loc <= end()   );
	
	const size_t n_bytes_to_move = (char*) end() - (char*) loc;
	const size_t n_bytes_to_void = n * sizeof (Box);
	
	const size_t offset = loc - begin();
	
	expand_by( n );
	
	loc = begin() + offset;  // In case we reallocated
	
	memmove( loc + n, loc, n_bytes_to_move );
	
	/*
		Void the inserted bytes by setting them to 0x7f rather than 0x00.
		The latter (which encodes a short string of NUL bytes) is fine for
		cells beyond the length of the container, but those within should be
		undefined values.
	*/
	
	memset( loc, Box_undefined, n_bytes_to_void );
	
	return loc;
}

void Container::erase_n( Item* loc, size_t n )
{
	ASSERT( n   <= size()    );
	ASSERT( loc >= begin()   );
	ASSERT( loc <= end()     );
	ASSERT( loc <= end() - n );
	
	const size_t n_bytes_to_move = (char*) end() - (char*) (loc + n);
	const size_t n_bytes_to_zero = n * sizeof (Box);
	
	memmove( loc, loc + n, n_bytes_to_move );
	
	memset( end() - n, '\0', n_bytes_to_zero );
	
	u.str.length -= n;
}

}
