/*	=============
 *	HexStrings.hh
 *	=============
 *	
 *	No implementation necessary.
 */

#pragma once

#include <string>

#include "HexCodes.hh"


namespace BitsAndBytes
{
	
	static std::string EncodeAsHex( const void* data, std::size_t size )
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
	
	static unsigned char JuxtaposeNibbles( char high, char low )
	{
		unsigned char result = high;
		
		result <<= 4;
		result |= low;
		
		return result;
	}
	
	static unsigned char DecodeHexNibbles( char high, char low )
	{
		return JuxtaposeNibbles( HexAsNibble( high ),
		                         HexAsNibble( low  ) );
	}
	
	static std::string DecodeHex( const std::string hexCodes )
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

