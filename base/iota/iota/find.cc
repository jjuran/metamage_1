/*
	iota/find.cc
	------------
	
	Copyright 2010, Joshua Juran
*/

#include "iota/find.hh"


namespace iota
{
	
	static inline bool char_matches( char a, char b )
	{
		return a == b;
	}
	
	const char* find_first_match( const char* p, unsigned length, char c )
	{
		for ( const char* end = p + length;  p != end;  ++p )
		{
			if ( char_matches( *p, c ) )
			{
				return p;
			}
		}
		
		return 0;  // NULL
	}
	
	const char* find_last_match( const char* p, unsigned length, char c )
	{
		const char* begin = p;
		
		p += length;
		
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
	                              unsigned              length,
	                              const unsigned char*  chars )
	{
		for ( const char* end = p + length;  p != end;  ++p )
		{
			if ( char_matches( *p, chars ) )
			{
				return p;
			}
		}
		
		return 0;  // NULL
	}
	
	const char* find_last_match( const char*           p,
	                             unsigned              length,
	                             const unsigned char*  chars )
	{
		const char* begin = p;
		
		p += length;
		
		while ( p != begin )
		{
			if ( char_matches( *--p, chars ) )
			{
				return p;
			}
		}
		
		return 0;  // NULL
	}
	
}

