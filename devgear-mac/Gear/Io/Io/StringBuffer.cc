/*	===============
 *	StringBuffer.cc
 *	===============
 */

#include "Io/StringBuffer.hh"

// Standard C++
#include <algorithm>

// Io
#include "Io/Exceptions.hh"


namespace Io
{
	
	int StringBuffer::Read( char* data, std::size_t byteCount )
	{
		if ( byteCount == 0 )
		{
			return 0;
		}
		
		if ( fData.empty() )
		{
			throw Io::NoDataPending();
		}
		
		// Don't copy more than (a) fits in the dest buffer, or (b) we have available.
		std::size_t bytesCopied = std::min( byteCount, fData.size() );
		
		// Copy to output buffer.
		std::copy( fData.begin(),
		           fData.begin() + bytesCopied,
		           data );
		
		// Move remainder of source (if any) to the beginning.
		std::copy( fData.begin() + bytesCopied,
		           fData.end(),
		           fData.begin() );
		
		fData.resize( fData.size() - bytesCopied );
		
		return bytesCopied;
	}
	
	int StringBuffer::Write( const char* data, std::size_t byteCount )
	{
		fData.append( data, byteCount );
		
		return byteCount;
	}
	
}

