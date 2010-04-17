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

// debug
#include "debug/assert.hh"


#define LENGTH_ERROR_MESSAGE  "plus::var_string size can't exceed 0x7fffffff"


namespace plus
{
	
	char* var_string::erase( char* p, char* q )
	{
		const size_type old_size = size();
		
		char* data = mutable_data();
		char* end  = data + old_size;
		
		ASSERT( data <= p   );
		ASSERT( p    <= q   );
		ASSERT( q    <= end );
		
		std::copy( q, end, p );
		
		const size_type new_size = old_size - ( q - p );
		
		set_length( new_size );
		
		return p;
	}
	
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
	
	void var_string::resize( size_type new_size, char c )
	{
		check_size( new_size );
		
		const size_type old_size = size();
		
		if ( new_size > old_size )
		{
			size_type _capacity = capacity();
			
			if ( new_size > _capacity )
			{
				do
				{
					_capacity *= 2;
				}
				while ( new_size > _capacity );
				
				reserve( _capacity );
			}
			
			char* data = begin();
			
			std::fill( data + old_size, data + new_size, c );
		}
		
		set_length( new_size );
	}
	
	char* var_string::insert_uninitialized( char* p, size_type n )
	{
		const size_type old_size = size();
		const size_type new_size = old_size + n;
		
		char* data = mutable_data();
		char* end  = data + old_size;
		
		ASSERT( data <= p   );
		ASSERT( p    <= end );
		
		ASSERT( new_size >= old_size );  // catch integer overflow
		
		const size_type offset = p - data;
		
		reserve( new_size );
		
		data = mutable_data();
		
		p    = data + offset;
		end  = data + old_size;
		
		std::copy_backward( p, end, data + new_size );
		
		set_length( new_size );
		
		return p;
	}
	
	void var_string::insert( char* p, char* i, char* j )
	{
		ASSERT( i <= j );
		
		const size_type n = j - i;
		
		std::copy( i, j, insert_uninitialized( p, n ) );
	}
	
	void var_string::insert( char* p, size_type n, char c )
	{
		memset( insert_uninitialized( p, n ), c, n );
	}
	
	var_string& var_string::append( const char* p, size_type length )
	{
		check_size( length );
		
		if ( length )
		{
			ASSERT( p != NULL );
			
			ASSERT( p + length >= p );
		}
		
		const size_type old_size = size();
		
		resize( old_size + length );
		
		memcpy( mutable_data() + old_size, p, length );
		
		return *this;
	}
	
	var_string& var_string::append( const char* s )
	{
		ASSERT( s != NULL );
		
		const size_type length = strlen( s );
		
		return append( s, length );
	}
	
	var_string& var_string::append( size_type n, char c )
	{
		resize( size() + n, c );
		
		return *this;
	}
	
}

