/*
	string_common.cc
	----------------
*/

#include "plus/string_common.hh"

// Standard C++
#include <stdexcept>

// plus
#include "plus/datum_access.hh"
#include "plus/datum_alloc.hh"


#define LENGTH_ERROR_MESSAGE  "string size can't exceed 0x7fffffff"


namespace plus
{
	
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
		
		char* p = set_capacity( datum, capacity );
		
		p[ length ] = '\0';
	}
	
}

