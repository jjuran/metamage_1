/*	===================
 *	traceback_tables.cc
 *	===================
 */

#include "recall/traceback_tables.hh"


namespace recall
{
	
	typedef unsigned long UInt32;
	
	struct traceback_table
	{
		unsigned long   zero;
		unsigned long   unknown1;
		unsigned long   unknown2;
		unsigned long   codeSize;
		unsigned short  nameSize;
		const char      name[2];
	};
	
	
	const traceback_table* find_symbol_name( return_address_traceback addr )
	{
		if ( addr == NULL )
		{
			return NULL;
		}
		
		const UInt32* word = (const UInt32*) addr;
		
		const UInt32* end  = word + 0x10000;
		
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
		return std::string( table->name, table->nameSize );
	}
	
}

