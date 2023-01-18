/*
	json/decode_number.cc
	---------------------
*/

#include "vxs/lib/json/decode_number.hh"

// Standard C
#include <math.h>

// iota
#include "iota/char_types.hh"

// vxo
#include "vxo/error.hh"
#include "vxo/number.hh"


#pragma exceptions off


namespace vxo
{
namespace json
{

Box decode_number( const char*& p )
{
	bool negative = false;
	
	char c = *p++;
	
	if ( c == '-' )
	{
		negative = true;
		
		c = *p++;
	}
	
	if ( ! iota::is_digit( c ) )
	{
		return Error( "expected a digit after minus sign in number" );
	}
	
	double result = 0;
	
	do
	{
		result *= 10;
		result += c - '0';
	}
	while ( iota::is_digit( c = *p++ ) );
	
	if ( c == '.' )
	{
		double decimal_factor = 1.0;
		
		c = *p++;
		
		if ( ! iota::is_digit( c ) )
		{
			return Error( "expected a digit after decimal point in number" );
		}
		
		do
		{
			result += (c - '0') * (decimal_factor /= 10);
		}
		while ( iota::is_digit( c = *p++ ) );
	}
	
	if ( (c | ' ') == 'e' )
	{
		c = *p++;
		
		bool negative_exponent = false;
		
		switch ( c )
		{
			case '-':
				negative_exponent = true;
				// fall through
			case '+':
				c = *p++;
				break;
			
			default:
				break;
		}
		
		if ( ! iota::is_digit( c ) )
		{
			return Error( "expected a digit for exponent in number" );
		}
		
		double exponent = 0;
		
		do
		{
			exponent *= 10;
			exponent += c - '0';
		}
		while ( iota::is_digit( c = *p++ ) );
		
		if ( negative_exponent )
		{
			exponent = -exponent;
		}
		
		result *= pow( 10, exponent );
	}
	
	if ( negative )
	{
		result = -result;
	}
	
	--p;
	
	return Number( result );
}

}
}
