/*	=============
 *	HexStrings.hh
 *	=============
 */

#ifndef BITSANDBYTES_HEXSTRINGS_HH
#define BITSANDBYTES_HEXSTRINGS_HH

#include <string>

#include "HexCodes.hh"


namespace BitsAndBytes
{
	
	std::string EncodeAsHex( const void* data, std::size_t size );
	
	template < class Data >
	std::string EncodeAsHex( const Data& data )
	{
		return EncodeAsHex( &data, sizeof data );
	}
	
	// E.g. 0x2B -> "2b"
	inline std::string ByteAsHex( unsigned char c )
	{
		/*
		return std::string() + NibbleAsHex( HighNibble( c ) )
		                     + NibbleAsHex( LowNibble ( c ) );
		*/
		
		return EncodeAsHex( c );
	}
	
	// E.g. 0x02, 0x0B -> 0x2b
	inline unsigned char JuxtaposeNibbles( char high, char low )
	{
		unsigned char result = high;
		
		result <<= 4;
		result |= low;
		
		return result;
	}
	
	// E.g. '2', 'b' -> 0x2b
	inline unsigned char DecodeHexNibbles( char high, char low )
	{
		return JuxtaposeNibbles( HexAsNibble( high ),
		                         HexAsNibble( low  ) );
	}
	
	// E.g. "666f6f20626171" -> "foo bar"
	std::string DecodeHex( const std::string hexCodes );
	
}

#endif

