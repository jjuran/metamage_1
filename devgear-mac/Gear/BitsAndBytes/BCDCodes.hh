/*	===========
 *	BCDCodes.hh
 *	===========
 */

#ifndef BITSANDBYTES_BCDCODES_HH
#define BITSANDBYTES_BCDCODES_HH

#include "HexCodes.hh"


namespace BitsAndBytes
{
	
	inline unsigned char DecodeBCD( unsigned char bcd )
	{
		char tens = HighNibble( bcd );
		char units = LowNibble( bcd );
		
		unsigned char result = tens * 10 + units;
		
		return result;
	}
	
}

#endif

