/*	==================
 *	macsbug_symbols.cc
 *	==================
 */

#include "recall/macsbug_symbols.hh"

// Standard C
#include <stdint.h>


namespace recall
{
	
	struct macsbug_symbol
	{
		unsigned char bytes[1];
	};
	
	
	const macsbug_symbol* find_symbol_name( return_address_68k addr )
	{
		if ( addr == NULL )
		{
			return NULL;
		}
		
		const uint16_t* word = (const uint16_t*) addr;
		
		const uint16_t* end  = word + 0x10000;
		
		while ( word < end )
		{
			if ( *word == 0x4e75 )
			{
				const uint8_t* p = (const uint8_t*) ++word;
				
				if ( (*p & 0x80) != 0x80 )
				{
					break;
				}
				
				return (const macsbug_symbol*) p;
			}
			
			++word;
		}
		
		return NULL;
	}
	
	
	std::string get_symbol_string( const macsbug_symbol* symbol )
	{
		const unsigned char* p = symbol->bytes;
		
		bool hasLongName = *p == 0x80;
		
		uint8_t length = hasLongName ? *++p : *p & 0x1f;
		
		return std::string( (const char*) p + 1, length );
	}
	
}

