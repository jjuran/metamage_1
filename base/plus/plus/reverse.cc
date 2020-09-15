/*
	reverse.cc
	----------
*/

#include "plus/reverse.hh"

// plus
#include "plus/var_string.hh"


namespace plus
{
	
	void reverse( char* begin, char* end )
	{
		char* p = begin;
		char* q = end;
		
		while ( p < q )
		{
			// Decrement q, swap *p and *q, increment p.
			const char c = *--q;
			*q = *p;
			*p++ = c;
		}
	}
	
	void reverse_in_place( string& s )
	{
		var_string vs = s.move();
		
		reverse( vs.begin(), vs.size() );
		
		s = vs.move();
	}
	
}
