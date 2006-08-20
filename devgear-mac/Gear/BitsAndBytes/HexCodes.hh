/*	===========
 *	HexCodes.hh
 *	===========
 */

#ifndef BITSANDBYTES_HEXCODES_HH
#define BITSANDBYTES_HEXCODES_HH


namespace BitsAndBytes
{
	
	// E.g. 0x4A -> 0x04
	inline char HighNibble( unsigned char c )
	{
		return c >> 4;
	}
	
	// E.g. 0x4A -> 0x0A
	inline char LowNibble( unsigned char c )
	{
		return c % (1 << 4);
	}
	
	// E.g. 0x02 -> '2', 0x0B -> 'b'
	inline char NibbleAsHex( char c )
	{
		return (c < 10) ? c + '0'
		                : c - 10 + 'a';
	}
	
	// E.g. '4' -> 0x04, 'a' -> 0x0A
	inline char HexAsNibble( char c )
	{
		// Here's the logic:
		// If c >= 'A' then it's alphabetic.
		//     ('a' - 'A') yields the lowercase bit.
		//     We OR that with c to force it lowercase.
		//     Then subtract 'a' to normalize to zero.
		//     Then add 10 because 0xA == 10, not zero.
		// Otherwise, c is a decimal digit.
		//     Just subtract '0' to normalize to zero.
		
		// test if c is alphabetic or numeric
		return (c >= 'A') ? ( c | ('a' - 'A') ) - 'a' + 10  // OR with lc bit, norm to 0 + 10
		                  : c - '0';                        // otherwise, normalize to zero
	}
	
}

#endif

