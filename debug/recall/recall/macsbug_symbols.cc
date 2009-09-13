/*	=================
 *	MacsbugSymbols.cc
 *	=================
 */

#include "recall/macsbug_symbols.hh"


namespace Backtrace
{
	
	typedef unsigned short UInt16;
	typedef unsigned char  UInt8;
	
	
	MacsbugSymbolPtr FindSymbolName( ReturnAddr68K addr )
	{
		if ( addr == NULL )
		{
			return NULL;
		}
		
		const UInt16* word = (const UInt16*) addr;
		
		const UInt16* end  = word + 0x10000;
		
		while ( word < end )
		{
			if ( *word == 0x4e75 )
			{
				const UInt8* p = (const UInt8*) ++word;
				
				if ( (*p & 0x80) != 0x80 )
				{
					break;
				}
				
				return (MacsbugSymbolPtr) p;
			}
			
			++word;
		}
		
		return NULL;
	}
	
	
	std::string GetSymbolString( MacsbugSymbolPtr symbol )
	{
		const unsigned char* p = symbol->bytes;
		
		bool hasLongName = *p == 0x80;
		
		UInt8 length = hasLongName ? *++p : *p & 0x1f;
		
		return std::string( (const char*) p + 1, length );
	}
	
}

