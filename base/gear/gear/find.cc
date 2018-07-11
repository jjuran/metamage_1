/*
	gear/find.cc
	------------
*/

#include "gear/find.hh"

// Standard C
#include <string.h>


namespace gear
{
	
	bool char_matches( char c, const unsigned char* chars )
	{
		for ( int n = *chars++;  n != 0;  --n )
		{
			if ( c == char( *chars++ ) )
			{
				return true;
			}
		}
		
		return false;
	}
	
	const char* find_first_match( const char*  p,
	                              const char*  end,
	                              const char*  sub,
	                              unsigned     sub_length,
	                              const char*  _default )
	{
		for ( end -= sub_length;  p <= end;  ++p )
		{
			if ( memcmp( p, sub, sub_length ) == 0 )
			{
				return p;
			}
		}
		
		return _default;
	}
	
	const char* find_last_match( const char*  p,
	                             const char*  end,
	                             const char*  sub,
	                             unsigned     sub_length,
	                             const char*  _default )
	{
		const char* begin = p;
		
		p = end - sub_length;
		
		while ( p >= begin )
		{
			if ( memcmp( p, sub, sub_length ) == 0 )
			{
				return p;
			}
			
			--p;
		};
		
		return _default;
	}
	
}
