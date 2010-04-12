/*
	var_string.cc
	-------------
	
	Copyright 2010, Joshua Juran
*/

#include "plus/var_string.hh"

// Standard C++
#include <algorithm>
#include <stdexcept>

// Standard C
#include <string.h>


#define LENGTH_ERROR_MESSAGE  "plus::var_string size can't exceed 0x7fffffff"


namespace plus
{
	
	static void check_size( string::size_type size )
	{
		// 2 GB limit on 32-bit platforms
		
		if ( size > string::max_size() )
		{
			const bool _32bit = sizeof size == 4;
			
			const char* message = _32bit ? LENGTH_ERROR_MESSAGE
			                             : LENGTH_ERROR_MESSAGE "ffffffff";
			
			throw std::length_error( message );
		}
	}
	
	void var_string::reserve( size_type new_capacity )
	{
		check_size( new_capacity );
		
		const size_type data_size = size();
		
		if ( new_capacity < data_size )
		{
			new_capacity = data_size;
		}
		
		if ( new_capacity <= max_offset )
		{
			// If the new capacity is small, use the internal buffer
			
			if ( const char* external = external_data() )
			{
				// Reassign the external data to the internal buffer,
				// even if the old and new capacity match.
				
				assign( external, data_size );
			}
			else
			{
				// The data are already in the internal buffer, so do nothing
			}
			
			return;
		}
		
		const size_type old_capacity = capacity();
		
		if ( new_capacity != old_capacity )
		{
			char* buffer;
			
			try
			{
				buffer = (char*) ::operator new( new_capacity + 1 );
			}
			catch ( ... )
			{
				if ( new_capacity > old_capacity )
				{
					// Attempt to increase capacity failed
					throw;
				}
				
				// Attempt to decrease capacity failed -- do nothing
				return;
			}
			
			memcpy( buffer, data(), data_size + 1 );
			
			assign( buffer, data_size, delete_basic, new_capacity );
		}
	}
	
	void var_string::resize( size_type new_size )
	{
		check_size( new_size );
		
		size_type new_capacity = capacity();
		
		if ( new_size > new_capacity )
		{
			do
			{
				new_capacity *= 2;
			}
			while ( new_size > new_capacity );
			
			reserve( new_capacity );
			
			char* data = begin();
			
			std::fill( data + size(), data + new_size, '\0' );
		}
		
		set_length( new_size );
	}
	
}

