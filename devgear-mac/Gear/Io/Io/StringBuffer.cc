/*	===============
 *	StringBuffer.cc
 *	===============
 */

#include "Io/StringBuffer.hh"

// Standard C++
#include <algorithm>

// Io
#include "io/io.hh"


namespace Io
{
	
	int StringBuffer::Read( char* data, std::size_t byteCount )
	{
		if ( byteCount == 0 )
		{
			return 0;
		}
		
		if ( itsData.empty() )
		{
			throw io::no_input_pending();
		}
		
		// Don't copy more than (a) fits in the dest buffer, or (b) we have available.
		std::size_t bytesCopied = std::min( byteCount, itsData.size() );
		
		// Copy to output buffer.
		std::copy( itsData.begin(),
		           itsData.begin() + bytesCopied,
		           data );
		
		// Move remainder of source (if any) to the beginning.
		std::copy( itsData.begin() + bytesCopied,
		           itsData.end(),
		           itsData.begin() );
		
		itsData.resize( itsData.size() - bytesCopied );
		
		return bytesCopied;
	}
	
	int StringBuffer::Write( const char* data, std::size_t byteCount )
	{
		itsData.append( data, byteCount );
		
		return byteCount;
	}
	
}

