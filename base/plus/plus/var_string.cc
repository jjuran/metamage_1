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

// plus
#include "plus/string_details.hh"


#define LENGTH_ERROR_MESSAGE  "plus::var_string size can't exceed 0x7fffffff"


namespace plus
{
	
	char* var_string::mutable_data()
	{
		copy_on_write();
		
		return const_cast< char* >( data() );
	}
	
	char* var_string::end()
	{
		copy_on_write();
		
		return const_cast< char* >( string::end() );
	}
	
	char* var_string::erase_unchecked( char* p, size_type n )
	{
		const size_type old_size = size();
		
		char* begin = const_cast< char* >( data() );
		char* end   = begin + old_size;
		
		char* q = p + n;
		
		std::copy( q, end, p );
		
		const size_type new_size = old_size - n;
		
		set_length( new_size );
		
		return p;
	}
	
	var_string& var_string::erase( size_type pos, size_type n )
	{
		const size_type old_size = size();
		
		if ( pos > old_size )
		{
			throw std::out_of_range( __func__ );
		}
		
		n = std::min( n, old_size - pos );
		
		char* data = mutable_data();
		
		char* p = data + pos;
		
		erase_unchecked( p, n );
		
		return *this;
	}
	
	char* var_string::erase( char* p, char* q )
	{
		char* begin = const_cast< char* >( data() );
		char* end   = begin + size();
		
		ASSERT( begin <= p   );
		ASSERT( p     <= q   );
		ASSERT( q     <= end );
		
		const size_type n = q - p;
		
		return erase_unchecked( p, n );
	}
	
	char* var_string::embiggen( size_type new_length, size_type new_capacity )
	{
		ASSERT( new_length   <= max_size() );
		ASSERT( new_capacity <= max_size() );
		
		const size_type capacity_ = capacity();
		const size_type size_     = size();
		
		if ( new_capacity <= new_length )
		{
			const bool growing = new_length > capacity_;
			
			new_capacity = new_capacity ? new_length
			             : growing      ? std::max( size_ * 2, new_length )
			             :                capacity_;
		}
		
		new_capacity = adjusted_capacity( new_capacity );
		
		if ( new_capacity != capacity_ )
		{
			try
			{
				var_string temp;
				
				char* new_pointer = temp.reallocate( new_capacity );
				
				memcpy( new_pointer, data(), size() );
				
				swap( temp );
			}
			catch ( ... )
			{
				const bool increasing = new_capacity > capacity_;
				
				if ( increasing )
				{
					throw;
				}
				
				// Failure to decrease capacity is not an error
			}
		}
		
		char* data = mutable_data();  // copy on write
		
		set_length( new_length );
		
		return data;
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
		
		if ( new_capacity < max_offset )
		{
			new_capacity = max_offset;
		}
		
		embiggen( size(), new_capacity );
	}
	
	void var_string::resize( size_type new_size, char c )
	{
		check_size( new_size );
		
		const size_type old_size = size();
		
		char* new_pointer = embiggen( new_size );
		
		if ( new_size > old_size )
		{
			std::fill( new_pointer + old_size, new_pointer + new_size, c );
		}
	}
	
	char* var_string::insert_uninitialized( char* p, size_type n )
	{
		const size_type old_size = size();
		const size_type new_size = old_size + n;
		
		char* begin = const_cast< char* >( data() );
		char* end   = begin + old_size;
		
		ASSERT( begin <= p   );
		ASSERT( p     <= end );
		
		ASSERT( new_size >= old_size );  // catch integer overflow
		
		const size_type offset = p - begin;
		
		begin = embiggen( new_size );
		
		p    = begin + offset;
		end  = begin + old_size;
		
		std::copy_backward( p, end, begin + new_size );
		
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
		
		char* new_pointer = embiggen( old_size + length );
		
		memcpy( new_pointer + old_size, p, length );
		
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

