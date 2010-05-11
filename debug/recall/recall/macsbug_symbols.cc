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
		if ( addr == 0 )
		{
			return 0;  // NULL
		}
		
		const uint16_t* word = (const uint16_t*) addr;
		
		const uint16_t* end  = word + 0x10000;
		
		while ( word < end )
		{
			const uint16_t op = *word;
			
			if ( (op & 0xfffe) == 0x4e74 )
			{
				if ( op == 0x4e74 )
				{
					++word;  // RTD:  skip op parameter
				}
				else
				{
					// RTS
				}
				
				const uint8_t* p = (const uint8_t*) ++word;
				
				if ( (*p & 0x80) != 0x80 )
				{
					break;
				}
				
				return (const macsbug_symbol*) p;
			}
			
			++word;
		}
		
		return 0;  // NULL
	}
	
	
	const char* get_symbol_string( const macsbug_symbol* symbol )
	{
		const unsigned char* p = symbol->bytes;
		
		const bool has_long_name = *p == 0x80;
		
		const uint8_t length = has_long_name ? *++p : *p & 0x1f;
		
		const char* name = (const char*) p + 1;
		
		const char next = name[ length ];  // should be NUL
		
		return name;
	}
	
}

