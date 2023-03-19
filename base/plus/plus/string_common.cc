/*
	string_common.cc
	----------------
*/

#include "plus/string_common.hh"

// debug
#include "debug/assert.hh"

// vxo
#include "vxo/datum_access.hh"
#include "vxo/datum_alloc.hh"


#ifndef NULL
#define NULL  0L
#endif


namespace plus
{
	
	using vxo::datum_max_offset;
	using vxo::datum_storage;
	using vxo::set_capacity;
	using vxo::size;
	using vxo::string_check_size;
	
	
	char* string_reserve_nothrow( datum_storage& datum, long capacity )
	{
		if ( capacity < 0 )
		{
			return NULL;
		}
		
		const long length = size( datum );
		
		if ( capacity < datum_max_offset )
		{
			capacity = datum_max_offset;
		}
		
		if ( capacity < length )
		{
			capacity = length;
		}
		
		return set_capacity_nothrow( datum, capacity );
	}
	
	void string_reserve( datum_storage& datum, long capacity )
	{
		string_check_size( capacity );
		
		const long length = size( datum );
		
		if ( capacity < datum_max_offset )
		{
			capacity = datum_max_offset;
		}
		
		if ( capacity < length )
		{
			capacity = length;
		}
		
		set_capacity( datum, capacity );
	}
	
	void string_set_length( datum_storage& datum, long length, char* data )
	{
		ASSERT( length >= 0 );
		
		ASSERT( length <= capacity( datum ) );
		
		ASSERT( data == begin( datum ) );
		
		if ( is_small( datum ) )
		{
			datum.small[ datum_max_offset ] = datum_max_offset - length;
		}
		else
		{
			datum.alloc.length = length;
		}
		
		data[ length ] = '\0';
	}
	
}
