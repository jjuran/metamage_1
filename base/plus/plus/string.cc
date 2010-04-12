/*
	string.cc
	---------
	
	Copyright 2010, Joshua Juran
*/

#include "plus/string.hh"

// Standard C
#include <string.h>

// Traditional
#include <alloca.h>

// debug
#include "debug/assert.hh"


namespace plus
{
	
	static void concat( const char* a, string::size_type  a_size,
	                    const char* b, string::size_type  b_size,
	                    string& result )
	{
		const string::size_type total_size = a_size + b_size;
		
		char* buffer = (char*) alloca( total_size );
		
		memcpy( buffer,          a, a_size );
		memcpy( buffer + a_size, b, b_size );
		
		result.assign( buffer, total_size );
	}
	
	
	void string::dispose()
	{
		if ( its_alloc.length  &&  its_small_name[ max_offset ] < 0 )
		{
			delete [] its_alloc.pointer;
		}
	}
	
	string::string( const char* p, size_type length )
	{
		its_alloc.length = 0;
		
		its_small_name[ max_offset ] = 0;
		
		assign( p, length );
	}
	
	string::string( const char* p, const char* q )
	{
		its_alloc.length = 0;
		
		its_small_name[ max_offset ] = 0;
		
		assign( p, q );
	}
	
	string::string( const char* s )
	{
		its_alloc.length = 0;
		
		its_small_name[ max_offset ] = 0;
		
		assign( s );
	}
	
	string::~string()
	{
		dispose();
	}
	
	string::string( const string& other )
	{
		its_alloc.length = 0;
		
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
		
		return   its_alloc.length == 0 ? 0
		       : margin >= 0           ? max_offset - margin
		       :                         its_alloc.length;
	}
	
	const char* string::c_str() const
	{
		const bool allocated = its_alloc.length  &&  its_small_name[ max_offset ] < 0;
		
		return allocated ? its_alloc.pointer : its_small_name;
	}
	
	const char* string::data() const
	{
		return its_small_name[ max_offset ] < 0 ? its_alloc.pointer
		                                        : its_small_name;
	}
	
	void string::assign( const char* p, size_type length )
	{
		if ( length )
		{
			ASSERT( p != NULL );
			
			ASSERT( p + length >= p );
		}
		
		const bool allocated = its_alloc.length  &&  its_small_name[ max_offset ] < 0;
		
		char const *const old_pointer = allocated ? its_alloc.pointer : NULL;
		
		if ( length >= sizeof its_small_name )
		{
			// may throw
			char *const pointer = new char [ length + 1 ];
			
			memcpy( pointer, p, length );
			
			pointer[ length ] = '\0';
			
			its_alloc.pointer = pointer;
			its_alloc.length  = length;
			
			its_small_name[ max_offset ] = 0xff;
		}
		else
		{
			memcpy( its_small_name, p, length );
			
			its_small_name[ length     ] = '\0';
			its_small_name[ max_offset ] = max_offset - length;
		}
		
		delete [] old_pointer;
	}
	
	void string::assign( const char* s )
	{
		ASSERT( s != NULL );
		
		const size_type length = strlen( s );
		
		assign( s, length );
	}
	
	void string::append( const char* p, size_type length )
	{
		if ( length )
		{
			ASSERT( p != NULL );
			
			ASSERT( p + length >= p );
		}
		
		concat( data(), size(), p, length, *this );
	}
	
	void string::append( const char* s )
	{
		ASSERT( s != NULL );
		
		const size_type length = strlen( s );
		
		append( s, length );
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
		const string::size_type size = a.size();
		
		return size == b.size()  &&  memcmp( a.c_str(), b.c_str(), size ) == 0;
	}
	
	bool operator==( const string& a, const char* b )
	{
		return strcmp( a.c_str(), b ) == 0;
	}
	
	
	bool operator<( const string& a, const string& b )
	{
		return strcmp( a.c_str(), b.c_str() ) < 0;
	}
	
	bool operator<( const string& a, const char* b )
	{
		return strcmp( a.c_str(), b ) < 0;
	}
	
	bool operator<( const char* a, const string& b )
	{
		return strcmp( a, b.c_str() ) < 0;
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

