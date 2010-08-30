/*
	Genie/FS/sys/mac/keys.cc
	------------------------
*/

#include "Genie/FS/sys/mac/keys.hh"

// plus
#include "plus/var_string.hh"

// Nitrogen
#include "Nitrogen/Events.hh"

// Genie
#include "Genie/FS/append_hex_encoded_byte.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	plus::string sys_mac_keys::Read( const FSTree* )
	{
		plus::var_string result;
		
		N::GetKeys_Result keys = N::GetKeys();
		
		for ( unsigned i = 0;  i < 16;  ++i )
		{
			const UInt8 byte = keys.keyMapByteArray[i];
			
			for ( unsigned j = 0;  j < 8;  ++j )
			{
				const bool down = byte & (1 << j);
				
				if ( down )
				{
					const UInt8 keyCode = i * 8 + j;
					
					append_hex_encoded_byte( result, keyCode );
					
					result += '\n';
				}
			}
		}
		
		return result;
	}
	
}

