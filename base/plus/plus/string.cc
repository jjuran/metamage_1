/*
	string.cc
	---------
	
	Copyright 2010, Joshua Juran 
	
	Some of the find-related member functions were written using 
	boost/shmem/containers/string.hpp as a guide.
*/

#include "plus/string.hh"

// Standard C++
#include <algorithm>
#include <stdexcept>

// Standard C
#include <string.h>

// debug
#include "debug/assert.hh"

// plus
#include "plus/concat.hh"
#include "plus/string_details.hh"


namespace plus
{
	
	static int lexicographical_compare_3way( const char* a, const char* a_end,
	                                         const char* b, const char* b_end )
	{
		while ( a < a_end  &&  b < b_end )
		{
			if ( const int diff = *a++ - *b++ )
			{
				return diff;
			}
		}
		
		return (a != a_end) - (b != b_end);
	}
	
	static int string_compare( const char* a, size_t a_size,
	                           const char* b, size_t b_size )
	{
		return lexicographical_compare_3way( a, a + a_size,
		                                     b, b + b_size );
	}
	
	static int string_equal( const char* a, size_t a_size,
	                         const char* b, size_t b_size )
	{
		return a_size == b_size  &&  string_compare( a, a_size,
		                                             b, b_size ) == 0;
	}
	
	
	class matches_any
	{
		private:
			const char*  begin;
			const char*  end;
		
		public:
			matches_any( const char* a, const char* z ) : begin( a ), end( z )
			{
			}
			
			bool operator()( char c ) const
			{
				return std::find( begin, end, c ) != end;
			}
	};
	
	class matches_none
	{
		private:
			const char*  begin;
			const char*  end;
		
		public:
			matches_none( const char* a, const char* z ) : begin( a ), end( z )
			{
			}
			
			bool operator()( char c ) const
			{
				return std::find( begin, end, c ) == end;
			}
	};
	
	
	template < class Iter, class F >
	static Iter find_last_if( Iter begin, Iter end, F f )
	{
		ASSERT( begin <= end );
		
		Iter it = end;
		
		while ( --it >= begin )
		{
			if ( f( *it ) )
			{
				return it;
			}
		}
		
		return end;
	}
	
	
	static void dispose( const char* pointer, int _policy )
	{
		switch ( _policy )
		{
			case ~delete_shared:
			{
				pointer -= sizeof (size_t);
				
				// This casts away const, but it's only the characters that are
				// const, not the refcount.
				
				size_t& refcount = *(size_t*) pointer;
				
				if ( --refcount > 0 )
				{
					break;
				}
			}
			
			// fall through
			
			case ~delete_basic:
				delete pointer;
				break;
			
			default:
				break;
		}
	}
	
	
	void string::set_length( size_type length )
	{
		char& margin = its_small_name[ max_offset ];
		
		if ( margin < 0 )
		{
			its_alloc.length = length;
		}
		else
		{
			ASSERT( length <= max_offset );
			
			margin = max_offset - length;
		}
		
		char* p = margin < 0 ? (char*) its_alloc.pointer : its_small_name;
		
		p[ length ] = '\0';
	}
	
	string::string( const char*    p,
	                size_type      length,
	                delete_policy  policy,
	                size_type      capacity )
	{
		its_alloc.pointer  = p;
		its_alloc.length   = length;
		its_alloc.capacity = capacity;  // may be zero
		
		its_alloc._policy = ~policy;
	}
	
	string::string( const char* p, size_type length )
	{
		its_small_name[ max_offset ] = 0;
		
		assign( p, length );
	}
	
	string::string( const char* p, const char* q )
	{
		its_small_name[ max_offset ] = 0;
		
		assign( p, q );
	}
	
	string::string( const char* s )
	{
		its_small_name[ max_offset ] = 0;
		
		assign( s );
	}
	
	string::string( size_type n, char c )
	{
		its_small_name[ max_offset ] = 0;
		
		assign( n, c );
	}
	
	string::~string()
	{
		dispose( its_alloc.pointer, its_alloc._policy );
	}
	
	string::string( const string& other, size_type pos, size_type n )
	{
		its_small_name[ max_offset ] = 0;
		
		assign( other, pos, n );
	}
	
	string::string( const string& other )
	{
		its_small_name[ max_offset ] = 0;
		
		assign( other );
	}
	
	string& string::operator=( const string& other )
	{
		assign( other );
		
		return *this;
	}
	
	string::size_type string::length() const
	{
		const int margin = its_small_name[ max_offset ];
		
		return margin >= 0 ? max_offset - margin
		                   : its_alloc.length;
	}
	
	string::size_type string::capacity() const
	{
		const int margin = its_small_name[ max_offset ];
		
		return   margin >= 0        ? max_offset
		       : its_alloc.capacity ? its_alloc.capacity
		       :                      its_alloc.length;
	}
	
	const char* string::data() const
	{
		return its_alloc._policy < 0 ? its_alloc.pointer
		                             : its_small_name;
	}
	
	const char* string::end() const
	{
		const int margin = its_small_name[ max_offset ];
		
		return margin >= 0 ? its_small_name    + max_offset - margin
		                   : its_alloc.pointer + its_alloc.length;
	}
	
	string& string::assign( const char* p, size_type length, delete_policy policy, size_type capacity )
	{
		if ( length )
		{
			ASSERT( p != NULL );
			
			ASSERT( p + length >= p );
		}
		
		dispose( its_alloc.pointer, its_alloc._policy );
		
		its_alloc.pointer  = p;
		its_alloc.length   = length;
		its_alloc.capacity = capacity;  // may be zero
		
		its_alloc._policy = ~policy;
		
		return *this;
	}
	
	char* string::reallocate( size_type length )
	{
		char const *const old_pointer = its_alloc.pointer;
		
		const char old_policy = its_alloc._policy;
		
		char* new_pointer = NULL;
		
		if ( length >= sizeof its_small_name )
		{
			const size_type capacity = adjusted_capacity( length );
			
			const size_t buffer_length = sizeof (size_t) + capacity + 1;
			
			// may throw
			new_pointer = (char*) ::operator new( buffer_length );
			
			reinterpret_cast< size_t* >( new_pointer )[0] = 1;  // refcount
			
			new_pointer += sizeof (size_t);
			
			its_alloc.pointer  = new_pointer;
			its_alloc.length   = length;
			its_alloc.capacity = capacity;
			
			its_alloc._policy = ~delete_shared;
		}
		else
		{
			new_pointer = its_small_name;
			
			its_small_name[ max_offset ] = max_offset - length;
		}
		
		new_pointer[ length ] = '\0';
		
		dispose( old_pointer, old_policy );
		
		return new_pointer;
	}
	
	void string::copy_on_write()
	{
		const int _policy = its_alloc._policy;
		
		if ( _policy >= 0 )
		{
			return;  // small string
		}
		
		if ( its_alloc.capacity )
		{
			if ( _policy != ~delete_shared )
			{
				// Handoff with capacity specified -- we own this
				return;
			}
			
			const size_t refcount = ((size_t*) its_alloc.pointer)[ -1 ];
			
			ASSERT( refcount != 0 );
			
			if ( refcount == 1 )
			{
				// Shared with no others
				return;
			}
		}
		
		assign( its_alloc.pointer, its_alloc.length );
	}
	
	string& string::assign( const char* p, size_type length )
	{
		if ( length )
		{
			ASSERT( p != NULL );
			
			ASSERT( p + length >= p );
		}
		
		if ( empty() )
		{
			char* new_pointer = reallocate( length );
			
			memcpy( new_pointer, p, length );
		}
		else
		{
			// Always assign to a temporary and then swap.
			// This handles the cases of input occupying either our small buffer
			// or an allocated one.
			
			string temp;
			
			temp.assign( p, length );
			
			swap( temp );
		}
		
		return *this;
	}
	
	string& string::assign( const char* s )
	{
		ASSERT( s != NULL );
		
		const size_type length = strlen( s );
		
		return assign( s, length );
	}
	
	string& string::assign( size_type n, char c )
	{
		char* pointer = reallocate( n );
		
		memset( pointer, c, n );
		
		return *this;
	}
	
	string& string::assign( const string& other, size_type pos, size_type n )
	{
		const size_type other_size = other.size();
		
		if ( pos > other_size )
		{
			throw std::out_of_range( "plus::string" );
		}
		
		n = std::min( n, other.size() - pos );
		
		return assign( other.data() + pos, n );
	}
	
	string& string::assign( const string& other )
	{
		if ( other.its_alloc._policy >= ~delete_never )
		{
			// Either it's a small string, or it occupies static storage.
			// Either way, we perform a shallow copy.
			
			// If this is a self-assignment, then *we* are either a small string
			// or static storage, and dispose() does nothing.
			
			dispose( its_alloc.pointer, its_alloc._policy );
			
			std::copy( other.its_longs,
			           other.its_longs + buffer_size_in_longs,
			           its_longs );
		}
		else
		{
			assign( other.data(), other.size() );
		}
		
		return *this;
	}
	
	void string::swap( string& other )
	{
		long temp_longs[ buffer_size_in_longs ];
		
		memcpy( temp_longs,      other.its_longs, buffer_size );
		memcpy( other.its_longs, its_longs,       buffer_size );
		memcpy( its_longs,       temp_longs,      buffer_size );
	}
	
	
	string::size_type string::copy( char* buffer, size_type n, size_type pos ) const
	{
		const size_t size = length();
		
		if ( pos > size )
		{
			throw std::out_of_range( "plus::string" );
		}
		
		n = std::min( n, size - pos );
		
		memcpy( buffer, data() + pos, n );
		
		return n;
	}
	
	
	string::size_type string::find( const string& s, size_type pos ) const
	{
		return find( s.data(), pos, s.size() );
	}
	
	string::size_type string::find( const char* s, size_type pos ) const
	{
		return find( s, pos, strlen( s ) );
	}
	
	string::size_type string::find( const char* s, size_type pos, size_type n ) const
	{
		const size_t size = length();
		
		if ( pos > size  ||  pos + n > size )
		{
			return npos;
		}
		
		if ( n == 0 )
		{
			return pos;
		}
		
		const char* begin = data();
		const char* end   = begin + size;
		
		const char* it = std::search( begin + pos, end, s, s + n );
		
		return it != end ? it - begin : npos;
	}
	
	string::size_type string::find( char c, size_type pos ) const
	{
		return find( &c, pos, 1 );
	}
	
	
	string::size_type string::rfind( const string& s, size_type pos ) const
	{
		return rfind( s.data(), pos, s.size() );
	}
	
	string::size_type string::rfind( const char* s, size_type pos ) const
	{
		return rfind( s, pos, strlen( s ) );
	}
	
	string::size_type string::rfind( const char* s, size_type pos, size_type n ) const
	{
		const size_type size = length();
		
		if ( n > size )
		{
			return npos;
		}
		
		pos = std::min( pos, size - n );
		
		if ( n == 0 )
		{
			return pos;
		}
		
		const char* begin = data();
		const char* end   = begin + pos + n;
		
		const char* it = std::find_end( begin, end, s, s + n );
		
		return it != end ? it - begin : npos;
	}
	
	string::size_type string::rfind( char c, size_type pos ) const
	{
		return rfind( &c, pos, 1 );
	}
	
	
	string::size_type string::find_first_of( const string& s, size_type pos ) const
	{
		return find_first_of( s.data(), pos, s.size() );
	}
	
	string::size_type string::find_first_of( const char* s, size_type pos ) const
	{
		return find_first_of( s, pos, strlen( s ) );
	}
	
	string::size_type string::find_first_of( const char* s, size_type pos, size_type n ) const
	{
		const size_t size = length();
		
		if ( pos >= size )
		{
			return npos;
		}
		
		const char* begin = data();
		const char* end   = begin + size;
		
		const char* it = std::find_first_of( begin + pos, end, s, s + n );
		
		return it != end ? it - begin : npos;
	}
	
	string::size_type string::find_first_of( char c, size_type pos ) const
	{
		return find( c, pos );
	}
	
	
	string::size_type string::find_last_of( const string& s, size_type pos ) const
	{
		return find_last_of( s.data(), pos, s.size() );
	}
	
	string::size_type string::find_last_of( const char* s, size_type pos ) const
	{
		return find_last_of( s, pos, strlen( s ) );
	}
	
	string::size_type string::find_last_of( const char* s, size_type pos, size_type n ) const
	{
		const size_type size = length();
		
		if ( 1 > size )
		{
			return npos;
		}
		
		pos = std::min( pos, size - 1 );
		
		const char* begin = data();
		const char* end   = begin + pos + 1;
		
		const char* it = find_last_if( begin, end, matches_any( s, s + n ) );
		
		return it != end ? it - begin : npos;
	}
	
	string::size_type string::find_last_of( char c, size_type pos ) const
	{
		return find_last_of( &c, pos, 1 );
	}
	
	
	string::size_type string::find_first_not_of( const string& s, size_type pos ) const
	{
		return find_first_not_of( s.data(), pos, s.size() );
	}
	
	string::size_type string::find_first_not_of( const char* s, size_type pos ) const
	{
		return find_first_not_of( s, pos, strlen( s ) );
	}
	
	string::size_type string::find_first_not_of( const char* s, size_type pos, size_type n ) const
	{
		const size_t size = length();
		
		if ( pos >= size )
		{
			return npos;
		}
		
		const char* begin = data();
		const char* end   = begin + size;
		
		const char* it = std::find_if( begin + pos, end, matches_none( s, s + n ) );
		
		return it != end ? it - begin : npos;
	}
	
	string::size_type string::find_first_not_of( char c, size_type pos ) const
	{
		return find_first_not_of( &c, pos, 1 );
	}
	
	
	string::size_type string::find_last_not_of( const string& s, size_type pos ) const
	{
		return find_last_not_of( s.data(), pos, s.size() );
	}
	
	string::size_type string::find_last_not_of( const char* s, size_type pos ) const
	{
		return find_last_not_of( s, pos, strlen( s ) );
	}
	
	string::size_type string::find_last_not_of( const char* s, size_type pos, size_type n ) const
	{
		const size_type size = length();
		
		if ( 1 > size )
		{
			return npos;
		}
		
		pos = std::min( pos, size - 1 );
		
		const char* begin = data();
		const char* end   = begin + pos + 1;
		
		const char* it = find_last_if( begin, end, matches_none( s, s + n ) );
		
		return it != end ? it - begin : npos;
	}
	
	string::size_type string::find_last_not_of( char c, size_type pos ) const
	{
		return find_last_not_of( &c, pos, 1 );
	}
	
	
	int string::compare( const string& b ) const
	{
		return string_compare( data(), size(), b.data(), b.size() );
	}
	
	int string::compare( size_type a_pos, size_type a_n, const string& b ) const
	{
		const size_type a_size = size();
		
		if ( a_pos > a_size )
		{
			throw std::out_of_range( "plus::string" );
		}
		
		a_n = std::min( a_n, a_size - a_pos );
		
		return string_compare( data() + a_pos, a_n, b.data(), b.size() );
	}
	
	int string::compare( size_type a_pos, size_type a_n, const string& b, size_type b_pos, size_type b_n ) const
	{
		const size_type a_size = size();
		
		if ( a_pos > a_size )
		{
			throw std::out_of_range( "plus::string" );
		}
		
		a_n = std::min( a_n, a_size - a_pos );
		
		const size_type b_size = b.size();
		
		if ( b_pos > b_size )
		{
			throw std::out_of_range( "plus::string" );
		}
		
		b_n = std::min( b_n, b_size - b_pos );
		
		return string_compare( data() + a_pos, a_n, b.data() + b_pos, b_n );
	}
	
	int string::compare( const char* b ) const
	{
		return string_compare( data(), size(), b, strlen( b ) );
	}
	
	int string::compare( size_type a_pos, size_type a_n, const char* b, size_type b_n ) const
	{
		const size_type a_size = size();
		
		if ( a_pos > a_size )
		{
			throw std::out_of_range( "plus::string" );
		}
		
		a_n = std::min( a_n, a_size - a_pos );
		
		b_n = std::min( b_n, strlen( b ) );
		
		return string_compare( data(), size(), b, b_n );
	}
	
	
	bool operator==( const string& a, const string& b )
	{
		return string_equal( a.data(), a.size(), b.data(), b.size() );
	}
	
	bool operator==( const string& a, const char* b )
	{
		return string_equal( a.data(), a.size(), b, strlen( b ) );
	}
	
	
	bool operator<( const string& a, const string& b )
	{
		return string_compare( a.data(), a.size(), b.data(), b.size() ) < 0;
	}
	
	bool operator<( const string& a, const char* b )
	{
		return string_compare( a.data(), a.size(), b, strlen( b ) ) < 0;
	}
	
	bool operator<( const char* a, const string& b )
	{
		return string_compare( a, strlen( a ), b.data(), b.size() ) < 0;
	}
	
	
	string operator+( const string& a, const string& b )
	{
		return concat( a.data(), a.size(), b.data(), b.size() );
	}
	
	string operator+( const string& a, const char* b )
	{
		return concat( a.data(), a.size(), b, strlen( b ) );
	}
	
	string operator+( const char* a, const string& b )
	{
		return concat( a, strlen( a ), b.data(), b.size() );
	}
	
}

