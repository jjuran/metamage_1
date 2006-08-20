/*	===========
 *	BCDCodes.hh
 *	===========
 *	
 *	No implementation necessary.
 */

#pragma once

#include "HexCodes.hh"


namespace BitsAndBytes {
	
	inline unsigned char DecodeBCD(unsigned char bcd)
	{
		char tens = HighNibble(bcd);
		char units = LowNibble(bcd);
		
		unsigned char result = tens * 10 + units;
		
		return result;
	}
	
}

