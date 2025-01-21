/*
	octal.cc
	--------
*/

#include "bignum/octal.hh"


namespace bignum
{

integer decode_octal( const char* p, unsigned n )
{
	integer result;
	
	integer octal_power = 1;
	
	p += n;
	
	while ( n-- )
	{
		if ( char c = *--p - '0' )
		{
			integer digit = c;
		
			result += octal_power * digit;
		}
		
		octal_power *= 8;
	}
	
	return result;
}

}
