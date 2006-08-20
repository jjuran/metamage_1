/*	===============
 *	DecimalCodes.hh
 *	===============
 *	
 *	No implementation necessary.
 */

#pragma once


namespace BitsAndBytes {
	
	// E.g. '3' -> 3  (0x33 -> 0x03)
	inline char DecodeDecDigit(char c)
	{
		return c - '0';
	}
	
	// E.g. 7 -> '7'  (0x07 -> 0x37)
	inline char EncodeDecDigit(char d)
	{
		return d + '0';
	}
	
}

