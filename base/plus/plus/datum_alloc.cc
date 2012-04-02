/*
	datum_alloc.cc
	--------------
*/

#include "plus/datum_alloc.hh"

// Standard C
#include <stdlib.h>
#include <string.h>

// debug
#include "debug/assert.hh"

// plus
#include "plus/datum_access.hh"


namespace plus
{
	
	/*
		These can be changed to use malloc() and free() when we're ready to
		take advantage of realloc().
	*/
	
	char* datum_alloc( unsigned long size )
	{
		return (char*) ::operator new( size );
	}
	
	void datum_free( char* mem )
	{
		::operator delete( mem );
	}
	
	
	struct datum_alloc_header
	{
		size_t refcount;
	};
	
	static inline unsigned long adjusted_capacity( unsigned long capacity )
	{
		const int n_missing_bits_of_precision = 2;
		
		return capacity | (1 << n_missing_bits_of_precision) - 1;
	}
	
	char* allocate( datum_storage& datum, long length, long capacity )
	{
		ASSERT( length   >= 0 );
		ASSERT( capacity >= 0 );
		
		char* new_pointer;
		
		if ( capacity >= datum_buffer_size )
		{
			capacity = adjusted_capacity( capacity );
			
			const size_t buffer_length = sizeof (datum_alloc_header) + capacity + 1;
			
			// may throw
			datum_alloc_header* header = (datum_alloc_header*) ::operator new( buffer_length );
			
			header->refcount = 1;
			
			new_pointer = reinterpret_cast< char* >( header + 1 );
			
			datum.alloc.pointer  = new_pointer;
			datum.alloc.length   = length;
			datum.alloc.capacity = capacity;
			
			datum.small[ datum_max_offset ] = ~delete_shared;
		}
		else
		{
			new_pointer = datum.small;
			
			datum.small[ datum_max_offset ] = datum_max_offset - length;
		}
		
		return new_pointer;
	}
	
	char* allocate_data( datum_storage& datum, const char* p, long n )
	{
		char* q = allocate( datum, n );
		
		memcpy( q, p, n );
		
		return q;
	}
	
	static void dispose( const char* pointer, signed char _policy )
	{
		switch ( _policy )
		{
			case ~delete_shared:
			case ~delete_owned:
			{
				// This casts away const, but it's only the characters that are
				// const, not the header.
				
				datum_alloc_header* header = (datum_alloc_header*) pointer - 1;
				
				if ( --header->refcount > 0 )
				{
					break;
				}
				
				pointer = (char*) header;
			}
			
			// fall through
			
			case ~delete_basic:
				::operator delete( (void*) pointer );
				break;
			
			case ~delete_free:
				free( (void*) pointer );
				break;
			
			default:
				break;
		}
	}
	
	void destroy( datum_storage& datum )
	{
		dispose( datum.alloc.pointer, datum.small[ datum_max_offset ] );
	}
	
	char* reallocate( datum_storage& datum, long length )
	{
		datum_storage old = datum;
		
		char* new_pointer = allocate( datum, length );
		
		destroy( old );
		
		return new_pointer;
	}
	
	char* extend_capacity( datum_storage& datum, long new_capacity )
	{
		datum_storage old = datum;
		
		const long n = size( old );
		
		char* q = allocate( datum, n, new_capacity );
		
		const char* p = begin( old );
		
		memcpy( q, p, n );
		
		destroy( old );
		
		return q;
	}
	
	char* curtail_capacity( datum_storage& datum, long new_capacity )
	{
		try
		{
			return extend_capacity( datum, new_capacity );
		}
		catch ( ... )
		{
			// Failure to decrease capacity is not an error
		}
		
		return begin( datum );
	}
	
	char* set_capacity( datum_storage& datum, long new_capacity )
	{
		new_capacity = adjusted_capacity( new_capacity );
		
		const long old_capacity = capacity( datum );
		
		if ( new_capacity != old_capacity )
		{
			if ( new_capacity > old_capacity )
			{
				return extend_capacity( datum, new_capacity );
			}
			else
			{
				return curtail_capacity( datum, new_capacity );
			}
		}
		
		return begin( datum );
	}
	
	char* copy_on_write( datum_storage& datum, bool tainting )
	{
		if ( is_small( datum ) )
		{
			return datum.small;
		}
		
		char* p;
		
		if ( margin( datum ) == ~delete_shared )
		{
			datum_alloc_header* header = (datum_alloc_header*) datum.alloc.pointer - 1;
			
			ASSERT( header->refcount != 0 );
			
			if ( header->refcount == 1 )
			{
				p = const_cast< char* >( datum.alloc.pointer + alloc_substr_offset( datum ) );
				
				goto single;
			}
		}
		else if ( datum.alloc.capacity > 0 )
		{
			// delete_owned or read/write external buffer
			return const_cast< char* >( datum.alloc.pointer );
		}
		else
		{
			tainting = false;
		}
		
		p = extend_capacity( datum, datum.alloc.length );
		
	single:
		
		if ( tainting )
		{
			datum.small[ datum_max_offset ] = ~delete_owned;
		}
		
		return p;
	}
	
}

