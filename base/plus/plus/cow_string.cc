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

// more-libc
#include "more/string.h"

// debug
#include "debug/assert.hh"

// plus
#include "plus/string_details.hh"


#if defined(__MWERKS__)  &&  __MWERKS__ < 0x2300
	#define THROW( type, s )  throw std::exception()
#else
	#define THROW( type, s )  throw std::type( s )
#endif


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
		
		string_set_length( store, new_size, begin );
		
		return p;
	}
	
	cow_string& cow_string::erase( size_type pos, size_type n )
	{
		const size_type old_size = size();
		
		if ( pos > old_size )
		{
			THROW( out_of_range, "cow_string::erase" );
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
	
	char* cow_string::embiggen( size_type new_length )
	{
		string_check_size( new_length );
		
		char* data;
		
		if ( new_length > capacity() )
		{
			size_type new_capacity = adjusted_capacity( std::max( size() * 2, new_length ) );
			
			data = extend_capacity( store, new_capacity );
		}
		else
		{
			data = mutable_data();  // copy on write
		}
		
		string_set_length( store, new_length, data );
		
		return data;
	}
	
	void cow_string::resize( size_type new_size, char c )
	{
		// embiggen() will throw if new_size exceeds max_size()
		
		const size_type old_size = size();
		
		char* new_pointer = embiggen( new_size );
		
		if ( new_size > old_size )
		{
			std::fill( new_pointer + old_size, new_pointer + new_size, c );
		}
	}
	
	char* cow_string::insert_uninitialized( char* p, size_type n )
	{
		check_size( n );
		
		const size_type old_size = size();
		const size_type new_size = old_size + n;
		
		// embiggen() will throw if new_size exceeds max_size()
		
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
			THROW( out_of_range, "cow_string::insert" );
		}
		
		return insert( pos, s.data(), s.size() );
	}
	
	cow_string& cow_string::insert( size_type pos, const string& s, size_type offset, size_type n )
	{
		const size_type s_size = s.size();
		
		if ( pos > size()  ||  offset > s_size )
		{
			THROW( out_of_range, "cow_string::insert" );
		}
		
		n = std::min( n, s_size - offset );
		
		return insert( pos, s.data() + offset, n );
	}
	
	cow_string& cow_string::insert( size_type pos, const char* s, size_type n )
	{
		// insert_uninitialized() will throw if n exceeds max_size()
		
		if ( pos > size() )
		{
			THROW( out_of_range, "cow_string::insert" );
		}
		
		char* data = mutable_data();
		
		mempcpy( insert_uninitialized( data + pos, n ), s, n );
		
		return *this;
	}
	
	cow_string& cow_string::insert( size_type pos, const char* s )
	{
		return insert( pos, s, strlen( s ) );
	}
	
	cow_string& cow_string::insert( size_type pos, size_type n, char c )
	{
		// insert_uninitialized() will throw if n exceeds max_size()
		
		if ( pos > size() )
		{
			THROW( out_of_range, "cow_string::insert" );
		}
		
		char* data = mutable_data();
		
		memset( insert_uninitialized( data + pos, n ), c, n );
		
		return *this;
	}
	
	void cow_string::insert( char* p, const char* i, const char* j )
	{
		ASSERT( i <= j );
		
		const size_type n = j - i;
		
		// insert_uninitialized() will throw if n exceeds max_size()
		
		std::copy( i, j, insert_uninitialized( p, n ) );
	}
	
	void cow_string::insert( char* p, size_type n, char c )
	{
		// insert_uninitialized() will throw if n exceeds max_size()
		
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
		const size_type new_size = old_size + length;
		
		// embiggen() will throw if new_size exceeds max_size()
		
		char* new_pointer = embiggen( new_size );
		
		mempcpy( new_pointer + old_size, p, length );
		
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
			THROW( out_of_range, "cow_string::append" );
		}
		
		n = std::min( n, other_size - pos );
		
		return append( other.data() + pos, n );
	}
	
	
	char* cow_string::replace_setup( char* p, size_type m, difference_type delta )
	{
		// delta is signed and therefore can't exceed max_size()
		
		// insert_uninitialized() will throw if new size exceeds max_size()
		
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
			THROW( out_of_range, "cow_string::replace" );
		}
		
		return replace( pos, m, s.data(), s.size() );
	}
	
	cow_string& cow_string::replace( size_type pos, size_type m, const string& s, size_type offset, size_type n )
	{
		const size_type s_size = s.size();
		
		if ( pos > size()  ||  offset > s_size )
		{
			THROW( out_of_range, "cow_string::replace" );
		}
		
		n = std::min( n, s_size - offset );
		
		return replace( pos, m, s.data() + offset, n );
	}
	
	cow_string& cow_string::replace( size_type pos, size_type m, const char* s, size_type n )
	{
		check_size( n );
		
		const size_type old_size = size();
		
		if ( pos > old_size )
		{
			THROW( out_of_range, "cow_string::replace" );
		}
		
		m = std::min( m, old_size - pos );
		
		const difference_type delta = n - m;
		
		char* p = mutable_data() + pos;
		
		p = replace_setup( p, m, delta );
		
		mempcpy( p, s, n );
		
		return *this;
	}
	
	cow_string& cow_string::replace( size_type pos, size_type m, const char* s )
	{
		return replace( pos, m, s, strlen( s ) );
	}
	
	cow_string& cow_string::replace( size_type pos, size_type m, size_type n, char c )
	{
		check_size( n );
		
		const size_type old_size = size();
		
		if ( pos > old_size )
		{
			THROW( out_of_range, "cow_string::replace" );
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
		check_size( n );
		
		ASSERT( begin() <= p );
		
		ASSERT( q <= end() );
		
		ASSERT( p <= q );
		
		const char* j = i + n;
		
		ASSERT( j <= begin()  ||  i >= end() );
		
		const size_type m = q - p;
		
		const difference_type delta = n - m;
		
		p = replace_setup( p, m, delta );
		
		mempcpy( p, i, n );
	}
	
	void cow_string::replace( char* p, char* q, const char *s )
	{
		replace( p, q, s, strlen( s ) );
	}
	
	void cow_string::replace( char* p, char* q, size_type n, char c )
	{
		check_size( n );
		
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
