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
#include "plus/datum_storage.hh"


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
	
}

