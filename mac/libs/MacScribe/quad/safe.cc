/*
	safe.cc
	-------
*/

#include "quad/safe.hh"

// iota
#include "iota/char_types.hh"


#pragma exceptions off


namespace MacScribe
{
	
	static
	bool is_initial_safe( char c )
	{
		return iota::is_alnum( c )  ||  c == '_'  ||  c & 0x80;
	}
	
	static
	bool char_is_safe( char c )
	{
		return is_initial_safe( c )  ||  c == '!'  ||  c == '#'  ||  c == '.';
	}
	
	bool is_safe_quad( quad_t q )
	{
		const char a = q >> 24;
		const char b = q >> 16;
		const char c = q >>  8;
		const char d = q >>  0;
		
		int n_spaces = d != ' ' ? 0
		             : c != ' ' ? 1
		             : b != ' ' ? 2
		             : a != ' ' ? 3
		             :            4;
		
		bool safe = true;
		
		switch ( n_spaces )
		{
			case 0:
				safe = char_is_safe( d );
				
				// fall through
			
			case 1:
				safe &= char_is_safe( c );
				
				// fall through
			
			case 2:
				safe &= char_is_safe( b );
				
				// fall through
			
			case 3:
				safe &= is_initial_safe( a );
				break;
			
			case 4:
				safe = false;
		}
		
		return safe;
	}
	
}
