/*
	iota/find.cc
	------------
	
	Copyright 2010, Joshua Juran
*/

#include "iota/find.hh"

// Standard C
#include <string.h>


namespace iota
{
	
	static inline bool char_matches( char a, char b )
	{
		return a == b;
	}
	
	const char* find_first_match( const char* p, const char* end, char c )
	{
		for ( ;  p != end;  ++p )
		{
			if ( char_matches( *p, c ) )
			{
				return p;
			}
		}
		
		return 0;  // NULL
	}
	
	const char* find_last_match( const char* p, const char* end, char c )
	{
		const char* begin = p;
		
		p = end;
		
		while ( p != begin )
		{
			if ( char_matches( *--p, c ) )
			{
				return p;
			}
		}
		
		return 0;  // NULL
	}
	
	
	static bool char_matches( char c, const unsigned char* chars )
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
	
	const char* find_first_match( const char*           p,
	                              const char*           end,
	                              const unsigned char*  chars )
	{
		for ( ;  p != end;  ++p )
		{
			if ( char_matches( *p, chars ) )
			{
				return p;
			}
		}
		
		return 0;  // NULL
	}
	
	const char* find_last_match( const char*           p,
	                             const char*           end,
	                             const unsigned char*  chars )
	{
		const char* begin = p;
		
		p = end;
		
		while ( p != begin )
		{
			if ( char_matches( *--p, chars ) )
			{
				return p;
			}
		}
		
		return 0;  // NULL
	}
	
	const char* find_first_match( const char*  p,
	                              const char*  end,
	                              const char*  sub,
	                              unsigned     sub_length )
	{
		for ( end -= sub_length;  p <= end;  ++p )
		{
			if ( memcmp( p, sub, sub_length ) == 0 )
			{
				return p;
			}
		}
		
		return 0;  // NULL
	}
	
	const char* find_last_match( const char*  p,
	                             const char*  end,
	                             const char*  sub,
	                             unsigned     sub_length )
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
		
		return 0;  // NULL
	}
	
}

