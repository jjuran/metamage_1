/*
	string_common.cc
	----------------
*/

#include "plus/string_common.hh"

// Standard C++
#include <stdexcept>

// debug
#include "debug/assert.hh"

// vxo
#include "vxo/datum_access.hh"
#include "vxo/datum_alloc.hh"


#define LENGTH_ERROR_MESSAGE  "string size can't exceed 0x7fffffff"


namespace plus
{
	
	using vxo::datum_max_offset;
	using vxo::datum_storage;
	using vxo::set_capacity;
	using vxo::size;
	
	
	void string_check_size( long size )
	{
		// 2 GB limit on 32-bit platforms
		
		if ( size < 0 )
		{
			const bool _32bit = sizeof size == 4;
			
			const char* message = _32bit ? LENGTH_ERROR_MESSAGE
			                             : LENGTH_ERROR_MESSAGE "ffffffff";
			
			throw std::length_error( message );
		}
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
