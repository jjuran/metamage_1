/*
	cow_string.cc
	-------------
*/

#include "plus/cow_string.hh"

// Standard C++
#include <algorithm>
#include <stdexcept>

// Standard C
#include <string.h>

// debug
#include "debug/assert.hh"

// plus
#include "plus/string_details.hh"


namespace plus
{
	
	char* cow_string::end()
	{
		copy_on_write( true );
		
		return const_cast< char* >( string::end() );
	}
	
	char* cow_string::erase_unchecked( char* p, size_type n )
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
	
	cow_string& cow_string::erase( size_type pos, size_type n )
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
	
	char* cow_string::erase( char* p, char* q )
	{
		char* begin = const_cast< char* >( data() );
		char* end   = begin + size();
		
		ASSERT( begin <= p   );
		ASSERT( p     <= q   );
		ASSERT( q     <= end );
		
		const size_type n = q - p;
		
		return erase_unchecked( p, n );
	}
	
	char* cow_string::embiggen( size_type new_length, size_type new_capacity )
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
				cow_string temp;
				
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
	
	void cow_string::reserve( size_type new_capacity )
	{
		check_size( new_capacity );
		
		if ( new_capacity < max_offset )
		{
			new_capacity = max_offset;
		}
		
		embiggen( size(), new_capacity );
	}
	
	void cow_string::resize( size_type new_size, char c )
	{
		check_size( new_size );
		
		const size_type old_size = size();
		
		char* new_pointer = embiggen( new_size );
		
		if ( new_size > old_size )
		{
			std::fill( new_pointer + old_size, new_pointer + new_size, c );
		}
	}
	
	char* cow_string::insert_uninitialized( char* p, size_type n )
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
	
	cow_string& cow_string::insert( size_type pos, const string& s )
	{
		if ( pos > size() )
		{
			throw std::out_of_range( __func__ );
		}
		
		return insert( pos, s.data(), s.size() );
	}
	
	cow_string& cow_string::insert( size_type pos, const string& s, size_type offset, size_type n )
	{
		const size_type s_size = s.size();
		
		if ( pos > size()  ||  offset > s_size )
		{
			throw std::out_of_range( __func__ );
		}
		
		n = std::min( n, s_size - offset );
		
		return insert( pos, s.data() + offset, n );
	}
	
	cow_string& cow_string::insert( size_type pos, const char* s, size_type n )
	{
		if ( pos > size() )
		{
			throw std::out_of_range( __func__ );
		}
		
		char* data = mutable_data();
		
		memcpy( insert_uninitialized( data + pos, n ), s, n );
		
		return *this;
	}
	
	cow_string& cow_string::insert( size_type pos, const char* s )
	{
		return insert( pos, s, strlen( s ) );
	}
	
	cow_string& cow_string::insert( size_type pos, size_type n, char c )
	{
		if ( pos > size() )
		{
			throw std::out_of_range( __func__ );
		}
		
		char* data = mutable_data();
		
		memset( insert_uninitialized( data + pos, n ), c, n );
		
		return *this;
	}
	
	void cow_string::insert( char* p, char* i, char* j )
	{
		ASSERT( i <= j );
		
		const size_type n = j - i;
		
		std::copy( i, j, insert_uninitialized( p, n ) );
	}
	
	void cow_string::insert( char* p, size_type n, char c )
	{
		memset( insert_uninitialized( p, n ), c, n );
	}
	
	char* cow_string::insert( char* p, char c )
	{
		p = insert_uninitialized( p, 1 );
		
		*p = c;
		
		copy_on_write( true );
		
		return p;
	}
	
	cow_string& cow_string::append( const char* p, size_type length )
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
	
	cow_string& cow_string::append( const char* s )
	{
		ASSERT( s != NULL );
		
		const size_type length = strlen( s );
		
		return append( s, length );
	}
	
	cow_string& cow_string::append( size_type n, char c )
	{
		resize( size() + n, c );
		
		return *this;
	}
	
	cow_string& cow_string::append( const string& other, size_type pos, size_type n )
	{
		const size_type other_size = other.size();
		
		if ( pos > other_size )
		{
			throw std::out_of_range( __func__ );
		}
		
		n = std::min( n, other_size - pos );
		
		return append( other.data() + pos, n );
	}
	
	
	char* cow_string::replace_setup( char* p, size_type m, difference_type delta )
	{
		if ( delta == 0 )
		{
			return p;
		}
		
		char* q = p + m;
		
		if ( delta > 0 )
		{
			p = insert_uninitialized( q, delta ) - m;
		}
		else
		{
			erase_unchecked( q + delta, -delta );
		}
		
		return p;
	}
	
	cow_string& cow_string::replace( size_type pos, size_type m, const string& s )
	{
		if ( pos > size() )
		{
			throw std::out_of_range( __func__ );
		}
		
		return replace( pos, m, s.data(), s.size() );
	}
	
	cow_string& cow_string::replace( size_type pos, size_type m, const string& s, size_type offset, size_type n )
	{
		const size_type s_size = s.size();
		
		if ( pos > size()  ||  offset > s_size )
		{
			throw std::out_of_range( __func__ );
		}
		
		n = std::min( n, s_size - offset );
		
		return replace( pos, m, s.data() + offset, n );
	}
	
	cow_string& cow_string::replace( size_type pos, size_type m, const char* s, size_type n )
	{
		const size_type old_size = size();
		
		if ( pos > old_size )
		{
			throw std::out_of_range( __func__ );
		}
		
		m = std::min( m, old_size - pos );
		
		const difference_type delta = n - m;
		
		char* p = mutable_data() + pos;
		
		p = replace_setup( p, m, delta );
		
		memcpy( p, s, n );
		
		return *this;
	}
	
	cow_string& cow_string::replace( size_type pos, size_type m, const char* s )
	{
		return replace( pos, m, s, strlen( s ) );
	}
	
	cow_string& cow_string::replace( size_type pos, size_type m, size_type n, char c )
	{
		const size_type old_size = size();
		
		if ( pos > old_size )
		{
			throw std::out_of_range( __func__ );
		}
		
		m = std::min( m, old_size - pos );
		
		const difference_type delta = n - m;
		
		char* p = mutable_data() + pos;
		
		p = replace_setup( p, m, delta );
		
		memset( p, c, n );
		
		return *this;
	}
	
	void cow_string::replace( char* p, char* q, const string& s )
	{
		replace( p, q, s.data(), s.size() );
	}
	
	void cow_string::replace( char* p, char* q, const char *i, size_type n )
	{
		ASSERT( begin() <= p );
		
		ASSERT( q <= end() );
		
		ASSERT( p <= q );
		
		const char* j = i + n;
		
		ASSERT( j <= begin()  ||  i >= end() );
		
		const size_type m = q - p;
		
		const difference_type delta = n - m;
		
		p = replace_setup( p, m, delta );
		
		memcpy( p, i, n );
	}
	
	void cow_string::replace( char* p, char* q, const char *s )
	{
		replace( p, q, s, strlen( s ) );
	}
	
	void cow_string::replace( char* p, char* q, size_type n, char c )
	{
		ASSERT( begin() <= p );
		
		ASSERT( q <= end() );
		
		ASSERT( p <= q );
		
		const size_type m = q - p;
		
		const difference_type delta = n - m;
		
		p = replace_setup( p, m, delta );
		
		memset( p, c, n );
	}
	
	void cow_string::replace( char* p, char* q, const char *i, const char *j )
	{
		ASSERT( i <= j );
		
		const size_type n = j - i;
		
		replace( p, q, i, n );
	}
	
}

