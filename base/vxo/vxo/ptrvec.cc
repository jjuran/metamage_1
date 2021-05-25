/*
	ptrvec.cc
	---------
*/

#include "vxo/ptrvec.hh"

// Standard C
#include <string.h>

// debug
#include "debug/assert.hh"

// plus
#include "plus/datum_alloc.hh"
#include "plus/extent.hh"


namespace vxo
{

const size_t minimum_capacity = sizeof (plus::datum_storage);
const size_t minimum_elements = minimum_capacity / sizeof (PtrVec::Item);

PtrVec::PtrVec( size_t n_items )
{
	u.str.pointer  = NULL;
	u.str.length   = 0;
	u.str.capacity = 0;
	
	set_subtype_byte( Box_vector_anyptr );
	set_control_byte( Box_vector_anyptr );
	
	if ( n_items > 0 )
	{
		if ( n_items < minimum_elements )
		{
			n_items = minimum_elements;
		}
		
		const size_t n_bytes = n_items * sizeof (anyptr_t);
		
		if ( char* alloc = plus::extent_alloc_nothrow( n_bytes ) )
		{
			u.str.pointer  = alloc;
			u.str.capacity = n_items;
			
			set_control_byte( Box_shared );
		}
	}
}

anyptr_t* PtrVec::expand_by_nothrow( size_t n )
{
	const size_t original_length = u.str.length;
	
	ASSERT( u.str.length <= u.str.capacity );
	
	const size_t new_length = u.str.length + n;
	
	if ( new_length > u.str.capacity )
	{
		u.str.length   *= sizeof (anyptr_t);
		u.str.capacity *= sizeof (anyptr_t);
		
		size_t new_capacity = u.str.capacity * 2;
		
		if ( new_capacity < new_length * sizeof (anyptr_t) )
		{
			new_capacity = new_length * sizeof (anyptr_t);
		}
		
		char* alloc;
		
		if ( u.str.pointer == NULL )
		{
			if ( new_capacity < minimum_capacity )
			{
				new_capacity = minimum_capacity;
			}
			
			alloc = plus::extent_alloc_nothrow( new_capacity );
			
			u.str.pointer  = alloc;
			u.str.capacity = new_capacity;
			
			set_control_byte( Box_shared );
		}
		else
		{
			plus::datum_storage& storage = *(plus::datum_storage*) this;
			
			alloc = extend_capacity_nothrow( storage, new_capacity );
		}
		
		u.str.capacity /= sizeof (anyptr_t);
	}
	
	u.str.length = new_length;
	
	return begin() + original_length;
}

anyptr_t* PtrVec::expand_by( size_t n )
{
	const size_t original_length = u.str.length;
	
	ASSERT( u.str.length <= u.str.capacity );
	
	const size_t new_length = u.str.length + n;
	
	if ( new_length > u.str.capacity )
	{
		u.str.length   *= sizeof (anyptr_t);
		u.str.capacity *= sizeof (anyptr_t);
		
		size_t new_capacity = u.str.capacity * 2;
		
		if ( new_capacity < new_length * sizeof (anyptr_t) )
		{
			new_capacity = new_length * sizeof (anyptr_t);
		}
		
		char* alloc;
		
		if ( u.str.pointer == NULL )
		{
			if ( new_capacity < minimum_capacity )
			{
				new_capacity = minimum_capacity;
			}
			
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
		
		u.str.capacity /= sizeof (anyptr_t);
	}
	
	u.str.length = new_length;
	
	return begin() + original_length;
}

anyptr_t* PtrVec::insert_n_nothrow( Item* loc, size_t n )
{
	ASSERT( loc >= begin() );
	ASSERT( loc <= end()   );
	
	const size_t n_bytes_to_move = (char*) end() - (char*) loc;
	const size_t n_bytes_to_zero = n * sizeof (anyptr_t);
	
	const size_t offset = loc - begin();
	
	if ( ! expand_by_nothrow( n ) )
	{
		return NULL;
	}
	
	loc = begin() + offset;  // In case we reallocated
	
	memmove( loc + n, loc, n_bytes_to_move );
	
	memset( loc, '\0', n_bytes_to_zero );
	
	return loc;
}

anyptr_t* PtrVec::insert_n( Item* loc, size_t n )
{
	ASSERT( loc >= begin() );
	ASSERT( loc <= end()   );
	
	const size_t n_bytes_to_move = (char*) end() - (char*) loc;
	const size_t n_bytes_to_zero = n * sizeof (anyptr_t);
	
	const size_t offset = loc - begin();
	
	expand_by( n );
	
	loc = begin() + offset;  // In case we reallocated
	
	memmove( loc + n, loc, n_bytes_to_move );
	
	memset( loc, '\0', n_bytes_to_zero );
	
	return loc;
}

anyptr_t* PtrVec::insert_nothrow( Item* loc, const_iterator begin, const_iterator end )
{
	const size_t n = end - begin;
	
	anyptr_t* ptr = insert_n( loc, n );
	
	if ( ptr )
	{
		memcpy( ptr, begin, n * sizeof (anyptr_t) );
	}
	
	return ptr;
}

void PtrVec::insert( Item* loc, const_iterator begin, const_iterator end )
{
	const size_t n = end - begin;
	
	memcpy( insert_n( loc, n ), begin, n * sizeof (anyptr_t) );
}

void PtrVec::erase_n( Item* loc, size_t n )
{
	ASSERT( n   <= size()    );
	ASSERT( loc >= begin()   );
	ASSERT( loc <= end()     );
	ASSERT( loc <= end() - n );
	
	const size_t n_bytes_to_move = (char*) end() - (char*) (loc + n);
	
	memmove( loc, loc + n, n_bytes_to_move );
	
	u.str.length -= n;
}

bool operator==( const PtrVec& a, const PtrVec& b )
{
	if ( a.size() == b.size() )
	{
		const size_t byte_size = a.size() * sizeof (anyptr_t);
		
		return memcmp( a.begin(), b.begin(), byte_size ) == 0;
	}
	
	return false;
}

}
