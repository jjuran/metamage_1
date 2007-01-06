/*	============
 *	TextInput.cc
 *	============
 */

#include "Io/TextInput.hh"


namespace Io
{
	
	void TextInputBuffer::Receive( const char* data, std::size_t bytes )
	{
		const char* p = data;
		const char* q = p;
		const char* end = data + bytes;
		
		while ( q < end )
		{
			while ( (q < end)  &&  (*q != '\r')  &&  (*q != '\n') )  q++;
			
			myPartialData += std::string( p, q - p );
			
			if ( q < end )
			{
				// Avoid generating an extra blank line for LF after CR
				bool isCRLF = (p == q) && (myLastNewlineChar == '\r') && (*q == '\n');
				
				if ( !isCRLF )
				{
					myStrings.Write( myPartialData );
					myPartialData = "";
				}
				
				myLastNewlineChar = *q;
				p = ++q;
			}
		}
	}
	
	bool TextInputBuffer::WritePartialData()
	{
		ended = true;
		
		if ( !myPartialData.empty() )
		{
			myStrings.Write( myPartialData );
			myPartialData = "";
			
			return true;
		}
		
		return false;
	}
	
	bool TextInputAdapter::GetMore()
	{
		enum { bufSize = 512 };  // FIXME
		
		char data[ bufSize ];
		
		try
		{
			int bytes = input.Read( data, bufSize );  // result is always positive
			
			buffer.Receive( data, bytes );
			
			return true;
		}
		catch ( NoDataPending ) {}
		catch ( EndOfInput    )
		{
			return buffer.WritePartialData();
		}
		
		return false;
	}
	
}

