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
#include "plus/extent.hh"


namespace plus
{
	
	void construct_from_move_untaint_policy( datum_storage&  x,
	                                         datum_movable&  y,
	                                         signed char     _old_policy )
	{
		x = y;
		
		if ( margin( x ) < 0 )
		{
			// Detach any allocated strings; leave small strings alone
			y.small[ datum_max_offset ] = 0;
			
			// _old_policy is either ~delete_owned or 0
			
			if ( margin( x ) == _old_policy )
			{
				x.small[ datum_max_offset ] = ~delete_shared;
			}
		}
	}
	
	void assign_from_move( datum_storage& x, datum_movable& y )
	{
		destroy( x );
		
		construct_from_move( x, y );
	}
	
	static inline unsigned long adjusted_capacity( unsigned long capacity )
	{
		const int n_missing_bits_of_precision = 2;
		
		return capacity | (1 << n_missing_bits_of_precision) - 1;
	}
	
	static
	char* allocate_nothrow( datum_storage& datum, long length, long capacity )
	{
		ASSERT( length   >= 0      );
		ASSERT( capacity >= length );
		
		char* new_pointer;
		
		if ( capacity >= datum_buffer_size )
		{
			capacity = adjusted_capacity( capacity );
			
			// may return NULL
			new_pointer = extent_alloc_nothrow( capacity + 1 );  // includes NUL
			
			if ( new_pointer == NULL )
			{
				return NULL;
			}
			
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
		
		new_pointer[ length ] = '\0';
		
		return new_pointer;
	}
	
	static
	char* allocate( datum_storage& datum, long length, long capacity )
	{
		ASSERT( length   >= 0      );
		ASSERT( capacity >= length );
		
		char* new_pointer;
		
		if ( capacity >= datum_buffer_size )
		{
			capacity = adjusted_capacity( capacity );
			
			// may throw
			new_pointer = extent_alloc( capacity + 1 );  // includes NUL
			
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
		
		new_pointer[ length ] = '\0';
		
		return new_pointer;
	}
	
	static inline
	char* allocate_nothrow( datum_storage& datum, long length )
	{
		return allocate_nothrow( datum, length, length );
	}
	
	static inline
	char* allocate( datum_storage& datum, long length )
	{
		return allocate( datum, length, length );
	}
	
	char* allocate_data( datum_storage& datum, const char* p, long n )
	{
		char* q = allocate( datum, n );
		
		memcpy( q, p, n );
		
		return q;
	}
	
	void construct_from_copy( datum_storage& x, const datum_storage& y, bool taint )
	{
		if ( margin( y ) >= ~delete_shared + taint )
		{
			if ( margin( y ) == ~delete_shared )
			{
				extent_add_ref( y.alloc.pointer );
			}
			
			x = y;
		}
		else
		{
			allocate_data( x, y.alloc.pointer + alloc_substr_offset( y ), y.alloc.length );
		}
	}
	
	void assign_from_copy( datum_storage& x, const datum_storage& y, bool taint )
	{
		if ( &x != &y )
		{
			destroy( x );
			
			construct_from_copy( x, y, taint );
		}
	}
	
	static void dispose( const char* pointer, signed char _policy )
	{
		switch ( _policy )
		{
			case ~delete_shared:
			case ~delete_owned:
				extent_release( pointer );
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
	
	char* reallocate_nothrow( datum_storage& datum, long length )
	{
		datum_storage old = datum;
		
		char* new_pointer = allocate_nothrow( datum, length );
		
		destroy( old );
		
		return new_pointer;
	}
	
	char* reallocate( datum_storage& datum, long length )
	{
		datum_storage old = datum;
		
		char* new_pointer = allocate( datum, length );
		
		destroy( old );
		
		return new_pointer;
	}
	
	char* extend_capacity_nothrow( datum_storage& datum, long new_capacity )
	{
		datum_storage old = datum;
		
		const long n = size( old );
		
		char* q = allocate_nothrow( datum, n, new_capacity );
		
		if ( q != NULL )
		{
			const char* p = begin( old );
			
			memcpy( q, p, n );
			
			destroy( old );
		}
		
		return q;
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
			if ( extent_refcount( datum.alloc.pointer ) == 1 )
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
	
	unsigned long area( const datum_storage& datum )
	{
		if ( is_small( datum ) )
		{
			return sizeof (datum_storage);
		}
		
		return sizeof (datum_storage) + extent_area( datum.alloc.pointer );
	}
	
}
