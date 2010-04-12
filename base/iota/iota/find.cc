/*
	iota/find.cc
	------------
	
	Copyright 2010, Joshua Juran
*/

#include "iota/find.hh"


namespace iota
{
	
	const char* find_first_match( const char* p, unsigned length, char c )
	{
		for ( const char* end = p + length;  p != end;  ++p )
		{
			if ( *p == c )
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
			if ( *--p == c )
			{
				return p;
			}
		}
		
		return 0;  // NULL
	}
	
}

