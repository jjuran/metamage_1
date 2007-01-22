/*	============
 *	TextInput.cc
 *	============
 */

#include "Io/TextInput.hh"


namespace Io
{
	
	void TextInputBuffer::WritePartialData()
	{
		itsStrings.Write( itsPartialData );
		itsPartialData = "";
	}
	
	void TextInputBuffer::ReceiveBlock( const char* data, std::size_t bytes )
	{
		const char* p   = data;
		const char* q   = data;
		const char* end = data + bytes;
		
		while ( q < end )
		{
			while ( (q < end)  &&  (*q != '\r')  &&  (*q != '\n') )  ++q;
			
			itsPartialData += std::string( p, q - p );
			
			if ( q < end )
			{
				// Avoid generating an extra blank line for LF after CR
				bool isCRLF = (p == q) && (itsLastNewlineChar == '\r') && (*q == '\n');
				
				if ( !isCRLF )
				{
					WritePartialData();
				}
				
				itsLastNewlineChar = *q;
				p = ++q;
			}
		}
	}
	
	bool TextInputBuffer::ReceiveEndOfInput()
	{
		itHasReachedEndOfInput = true;
		
		if ( !itsPartialData.empty() )
		{
			WritePartialData();
			
			return true;
		}
		
		return false;
	}
	
}

