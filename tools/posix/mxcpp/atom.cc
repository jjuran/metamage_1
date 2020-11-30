/*
	atom.cc
	-------
*/

#include "atom.hh"

// Standard C
#include <ctype.h>

// mxcpp
#include "exception.hh"
#include "expression.hh"
#include "numeral.hh"


namespace tool
{
	
	static value_t eval_character_constant( const char* p )
	{
		value_t result = 0;
		
		while ( *++p != '\'' )
		{
			if ( *p == '\\' )
			{
				++p;
			}
			
			result = result << 8 | *p;
		}
		
		return result;
	}
	
	value_t eval_atom( const char* p )
	{
		if ( isdigit( *p ) )
		{
			return eval_numeral( p );
		}
		
		if ( *p == '\'' )
		{
			return eval_character_constant( p );
		}
		
		if ( isalpha( *p )  ||  *p == '_' )
		{
			return 0;
		}
		
		throw exception( "bad_preprocessor_atom" );
	}
	
}
