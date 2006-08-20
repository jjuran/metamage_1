/*	=================
 *	DecimalStrings.hh
 *	=================
 */

#ifndef BITSANDBYTES_DECIMALSTRINGS_HH
#define BITSANDBYTES_DECIMALSTRINGS_HH

#include <string>

#include "DecimalCodes.hh"


namespace BitsAndBytes
{
	
	std::string EncodeDecimal_( unsigned long number );
	
	inline std::string EncodeDecimal( unsigned long number )
	{
		return number == 0 ? "0"
		                   : EncodeDecimal_( number );
	}
	
	inline std::string EncodeInteger( long number )
	{
		return number < 0 ? std::string( "-" ) + EncodeDecimal_( -number )
		                  :                      EncodeDecimal (  number );
	}
	
	// E.g.   76 -> "76"
	// E.g. 1984 -> "84"
	inline std::string EncodeDecimal2( unsigned int number )
	{
		char str[3] = "to";
		
		str[0] = EncodeDecDigit( number / 10 );
		str[1] = EncodeDecDigit( number % 10 );
		
		return str;
	}
	
	// E.g.   76 -> "0076"
	// E.g. 1984 -> "1984"
	inline std::string EncodeDecimal4( unsigned int number )
	{
		char str[5] = "four";
		
		str[0] = EncodeDecDigit( number / 1000 );
		str[1] = EncodeDecDigit( number / 100 % 10 );
		str[2] = EncodeDecDigit( number /  10 % 10 );
		str[3] = EncodeDecDigit( number       % 10 );
		
		return str;
	}
	
}

#endif

