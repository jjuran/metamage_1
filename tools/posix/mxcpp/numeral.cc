/*
	numeral.cc
	----------
*/

#include "numeral.hh"

// Standard C
#include <ctype.h>

// gear
#include "gear/hexadecimal.hh"

// mxcpp
#include "exception.hh"
#include "expression.hh"


namespace tool
{
	
	value_t eval_numeral( const char* p )
	{
		value_t result = 0;
		
		if ( *p == '0' )
		{
			++p;
			
			if ( (*p | ' ') == 'x' )
			{
				while ( isxdigit( *++p ) )
				{
					result = result << 4 | gear::decoded_hex_digit( *p );
				}
			}
			else if ( isdigit( *p ) )
			{
				while ( *p >= '0'  &&  *p < '8' )
				{
					result = result << 3 | *p++ - '0';
				}
			}
		}
		else
		{
			while ( isdigit( *p ) )
			{
				result = result * 10 + (*p++ - '0');
			}
		}
		
		while ( (*p | ' ') == 'u'  ||  (*p | ' ') == 'l' )
		{
			++p;
		}
		
		if ( isalnum( *p )  ||  *p == '_' )
		{
			throw exception( "bad_integer_constant" );
		}
		
		return result;
	}
	
}
