/*	=================
 *	DecimalStrings.cc
 *	=================
 */

#include "DecimalStrings.hh"


namespace BitsAndBytes
{
	
	// E.g.    0 ->     ""
	// E.g.   76 ->   "76"
	// E.g. 1984 -> "1984"
	std::string EncodeDecimal_( unsigned long number )
	{
		return number == 0 ? ""
		                   :   EncodeDecimal_( number / 10 )
		                     + EncodeDecDigit( number % 10 );
	}
	
}

