/*	=============
 *	HexStrings.cc
 *	=============
 */

#include "HexStrings.hh"


namespace BitsAndBytes
{
	
	// E.g. "foo bar" -> "666f6f20626171"
	std::string EncodeAsHex( const void* data, std::size_t size )
	{
		std::string result;
		
		result.resize( size * 2 );
		
		const unsigned char* bytes = reinterpret_cast< const unsigned char* >( data );
		
		for ( std::size_t i = 0;  i < size;  ++i )
		{
			result[ i * 2     ] = NibbleAsHex( HighNibble( bytes[ i ] ) );
			result[ i * 2 + 1 ] = NibbleAsHex( LowNibble ( bytes[ i ] ) );
		}
		
		return result;
	}
	
	// E.g. "666f6f20626171" -> "foo bar"
	std::string DecodeHex( const std::string hexCodes )
	{
		std::string result;
		
		// FIXME:  Verify the hex data.
		
		result.resize( hexCodes.size() / 2 );
		
		for ( std::size_t i = 0;  i < result.size();  ++i )
		{
			char high = hexCodes[ i * 2     ];
			char low  = hexCodes[ i * 2 + 1 ];
			
			result[ i ] = DecodeHexNibbles( high, low );
		}
		
		return result;
	}
	
}

