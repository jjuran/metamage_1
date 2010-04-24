/*
	string.cc
	---------
	
	Copyright 2010, Joshua Juran
*/

#include "plus/string.hh"

// Standard C
#include <string.h>

// debug
#include "debug/assert.hh"


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
	
	
	static void concat( const char* a, string::size_type  a_size,
	                    const char* b, string::size_type  b_size,
	                    string& result )
	{
		const string::size_type total_size = a_size + b_size;
		
		char* buffer = (char*) ::operator new( total_size + 1 );
		
		memcpy( buffer,          a, a_size );
		memcpy( buffer + a_size, b, b_size );
		
		buffer[ total_size ] = '\0';
		
		result.assign( buffer, total_size, delete_basic );
	}
	
	static void dispose( const char* pointer, int margin )
	{
		if ( margin == ~delete_basic )
		{
			delete pointer;
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
		its_alloc.capacity = capacity ? capacity : length;
		
		its_small_name[ max_offset ] = ~policy;
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
		dispose( its_alloc.pointer, its_small_name[ max_offset ] );
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
		
		return margin >= 0 ? max_offset
		                   : its_alloc.capacity;
	}
	
	const char* string::data() const
	{
		return its_small_name[ max_offset ] < 0 ? its_alloc.pointer
		                                        : its_small_name;
	}
	
	const char* string::internal_data() const
	{
		return its_small_name[ max_offset ] < 0 ? NULL
		                                        : its_small_name;
	}
	
	const char* string::external_data() const
	{
		return its_small_name[ max_offset ] < 0 ? its_alloc.pointer
		                                        : NULL;
	}
	
	string& string::assign( const char* p, size_type length, delete_policy policy, size_type capacity )
	{
		if ( length )
		{
			ASSERT( p != NULL );
			
			ASSERT( p + length >= p );
		}
		
		dispose( its_alloc.pointer, its_small_name[ max_offset ] );
		
		its_alloc.pointer  = p;
		its_alloc.length   = length;
		its_alloc.capacity = capacity ? capacity : length;
		
		its_small_name[ max_offset ] = ~policy;
		
		return *this;
	}
	
	char* string::reallocate( size_type length )
	{
		char const *const old_pointer = its_alloc.pointer;
		
		const char old_margin = its_small_name[ max_offset ];
		
		char* new_pointer = NULL;
		
		if ( length >= sizeof its_small_name )
		{
			// may throw
			new_pointer = (char*) ::operator new( length + 1 );
			
			its_alloc.pointer  = new_pointer;
			its_alloc.length   = length;
			its_alloc.capacity = length;
			
			its_small_name[ max_offset ] = ~delete_basic;
		}
		else
		{
			new_pointer = its_small_name;
			
			its_small_name[ max_offset ] = max_offset - length;
		}
		
		new_pointer[ length ] = '\0';
		
		dispose( old_pointer, old_margin );
		
		return new_pointer;
	}
	
	string& string::assign( const char* p, size_type length )
	{
		if ( length )
		{
			ASSERT( p != NULL );
			
			ASSERT( p + length >= p );
		}
		
		if ( p < its_small_name + sizeof its_small_name  &&  p + length > its_small_name )
		{
			// input lies within our small buffer
			
			string temp( p, length );
			
			swap( temp );
		}
		else
		{
			// in case the input lies in an allocated buffer
			string temp;
			
			swap( temp );
			
			char* new_pointer = reallocate( length );
			
			memcpy( new_pointer, p, length );
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
	
	void string::swap( string& other )
	{
		long temp_longs[ buffer_size_in_longs ];
		
		memcpy( temp_longs,      other.its_longs, buffer_size );
		memcpy( other.its_longs, its_longs,       buffer_size );
		memcpy( its_longs,       temp_longs,      buffer_size );
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
	
	
	string concat( const char* a, string::size_type  a_size,
	               const char* b, string::size_type  b_size )
	{
		string result;
		
		concat( a, a_size, b, b_size, result );
		
		return result;
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

