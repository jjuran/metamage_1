/*	============
 *	TextInput.cc
 *	============
 */

#include "Io/TextInput.hh"


namespace Io
{
	
	bool TextInputAdapter::GetMore()
	{
		enum { bufSize = 512 };  // FIXME
		
		char data[ bufSize ];
		
		try
		{
			int bytes = input.Read( data, bufSize );  // result is always positive
			
			char* p = data;
			char* q = p;
			char* end = data + bytes;
			
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
			
			return true;
		}
		catch ( NoDataPending ) {}
		catch ( EndOfInput    )
		{
			ended = true;
			
			if ( !myPartialData.empty() )
			{
				myStrings.Write( myPartialData );
				myPartialData = "";
				return true;
			}
		}
		
		return false;
	}
	
}

