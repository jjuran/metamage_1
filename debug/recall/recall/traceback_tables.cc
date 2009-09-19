/*	===================
 *	traceback_tables.cc
 *	===================
 */

#include "recall/traceback_tables.hh"

// Standard C
#include <stdint.h>


namespace recall
{
	
	struct traceback_table
	{
		uint32_t    zero;
		uint32_t    unknown1;
		uint32_t    unknown2;
		uint32_t    code_size;
		uint16_t    name_size;
		const char  name[2];
	};
	
	
	const traceback_table* find_symbol_name( return_address_traceback addr )
	{
		if ( addr == NULL )
		{
			return NULL;
		}
		
		const uint32_t* word = (const uint32_t*) addr;
		
		const uint32_t* end  = word + 0x10000;
		
		while ( word < end )
		{
			if ( *word == 0x4e800020 )
			{
				const traceback_table* table = (const traceback_table*) ++word;
				
				if ( table->zero != 0 )
				{
					break;
				}
				
				if ( table->name[0] != '.' )
				{
					break;
				}
				
				return table;
			}
			
			++word;
		}
		
		return NULL;
	}
	
	std::string get_symbol_string( const traceback_table* table )
	{
		return std::string( table->name, table->name_size );
	}
	
}

