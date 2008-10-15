/*	======================
 *	FSTree_sys_mac_keys.cc
 *	======================
 */

#include "Genie/FileSystem/FSTree_sys_mac_keys.hh"

// Nitrogen
#include "Nitrogen/Events.h"

// BitsAndBytes
#include "HexStrings.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	static void WriteKeys( int bit, UInt32 keys, std::string& result )
	{
		using BitsAndBytes::ByteAsHex;
		
		while ( keys )
		{
			if ( keys & 0x80000000 )
			{
				result += ByteAsHex( bit );
				result += '\n';
			}
			
			keys <<= 1;
			
			++bit;
		}
	}
	
	std::string sys_mac_keys_Query::operator()() const
	{
		std::string result;
		
		N::GetKeys_Result keys = N::GetKeys();
		
		WriteKeys( 0 * 32, keys.keyMap[ 0 ], result );
		WriteKeys( 1 * 32, keys.keyMap[ 1 ], result );
		WriteKeys( 2 * 32, keys.keyMap[ 2 ], result );
		WriteKeys( 3 * 32, keys.keyMap[ 3 ], result );
		
		return result;
	}
	
}

