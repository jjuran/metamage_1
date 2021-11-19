/*
	plus/mac_utf8.cc
	----------------
*/

#include "plus/mac_utf8.hh"

// chars
#include "conv/mac_utf8.hh"

// Debug
#include "debug/assert.hh"


namespace plus
{
	
	string::size_type sizeof_utf8_from_mac( const string& mac )
	{
		const char* begin = mac.begin();
		
		return conv::sizeof_utf8_from_mac( begin, begin + mac.size() );
	}
	
	string::size_type sizeof_mac_from_utf8( const string& utf8 )
	{
		const char* begin = utf8.begin();
		
		return conv::sizeof_mac_from_utf8( begin, begin + utf8.size() );
	}
	
	static
	bool has_non_ascii( const char* begin, const char* end )
	{
		while ( begin < end )
		{
			signed char c = *begin++;
			
			if ( c < 0 )
			{
				return true;
			}
		}
		
		return false;
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
		
		if ( ! has_non_ascii( begin, end ) )
		{
			return input;  // input is entirely ASCII
		}
		
		return utf8_from_mac( begin, input.size() );
	}
	
	string mac_from_utf8( const string& input )
	{
		const char* begin = input.data();
		const char* end   = begin + input.size();
		
		if ( ! has_non_ascii( begin, end ) )
		{
			return input;  // input is entirely ASCII
		}
		
		return mac_from_utf8( begin, input.size() );
	}
	
}
