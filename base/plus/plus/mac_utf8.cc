/*
	plus/mac_utf8.cc
	----------------
*/

#include "plus/mac_utf8.hh"

// Standard C++
#include <algorithm>
#include <functional>

// chars
#include "conv/mac_utf8.hh"

// Debug
#include "debug/assert.hh"


namespace plus
{
	
	static inline bool is_non_ascii( char c )
	{
		return c & 0x80;
	}
	
	string utf8_from_mac( const char* begin, string::size_type n )
	{
		const char* end = begin + n;
		
		const std::size_t measured_size = conv::sizeof_utf8_from_mac( begin, end );
		
		string result;
		
		char* p = result.reset( measured_size );
		
		const std::size_t actual_size = conv::utf8_from_mac( p, measured_size, begin, n );
		
		ASSERT( actual_size == measured_size );
		
		return result;
	}
	
	string mac_from_utf8( const char* begin, string::size_type n )
	{
		const char* end = begin + n;
		
		const std::size_t measured_size = conv::sizeof_mac_from_utf8( begin, end );
		
		string result;
		
		char* p = result.reset( measured_size );
		
		const std::size_t actual_size = conv::mac_from_utf8( p, measured_size, begin, n );
		
		ASSERT( actual_size == measured_size );
		
		return result;
	}
	
	
	string utf8_from_mac( const string& input )
	{
		const char* begin = input.data();
		const char* end   = begin + input.size();
		
		const char* it = std::find_if( begin,
		                               end,
		                               std::ptr_fun( is_non_ascii ) );
		
		if ( it == end )
		{
			return input;  // input is entirely ASCII
		}
		
		return utf8_from_mac( begin, input.size() );
	}
	
	string mac_from_utf8( const string& input )
	{
		const char* begin = input.data();
		const char* end   = begin + input.size();
		
		const char* it = std::find_if( begin,
		                               end,
		                               std::ptr_fun( is_non_ascii ) );
		
		if ( it == end )
		{
			return input;  // input is entirely ASCII
		}
		
		return mac_from_utf8( begin, input.size() );
	}
	
}

