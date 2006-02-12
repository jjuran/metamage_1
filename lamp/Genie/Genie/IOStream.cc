/*	===========
 *	IOStream.cc
 *	===========
 */

#include "Genie/IOStream.hh"

// Nucleus
#include "Nucleus/NAssert.h"


namespace Genie
{
	
	// Return a reference to the peek buffer with at least minBytes of data in it.
	// We Read() until we have enough data.  If there're not enough data available,
	// then we either block, or throw EWOULDBLOCK if the stream is non-blocking.
	
	const std::string& IOStream::Peek( ByteCount minBytes )
	{
		while ( fPeekBuffer.size() < minBytes )
		{
			const ByteCount kBufferLength = 512;
			
			char data[ kBufferLength ];
			
			int bytes = SysRead( data, kBufferLength );  // will block, throw, or return positive
			
			ASSERT( bytes > 0 );
			
			fPeekBuffer.append( data, bytes );
		}
		
		return fPeekBuffer;
	}
	
	unsigned int IOStream::Poll() const
	{
		return SysPoll() | (fPeekBuffer.empty() ? 0 : kPollRead);
	}
	
	int IOStream::Read( char* data, std::size_t byteCount )
	{
		ByteCount bytesRead = std::min( fPeekBuffer.size(), byteCount );
		
		if ( bytesRead > 0 )
		{
			std::copy( fPeekBuffer.begin(),
			           fPeekBuffer.begin() + bytesRead,
			           data );
			
			std::copy( fPeekBuffer.begin() + bytesRead,
			           fPeekBuffer.end(),
			           fPeekBuffer.begin() );
			
			fPeekBuffer.resize( fPeekBuffer.size() - bytesRead );
			
			data += bytesRead;
			byteCount -= bytesRead;
		}
		
		if ( byteCount > 0 )
		{
			try
			{
				int result = SysRead( data, byteCount );
				
				bytesRead += result;
			}
			catch ( ... )
			{
				if ( bytesRead == 0 )
				{
					throw;
				}
			}
		}
		
		return bytesRead;
	}
	
	int IOStream::Write( const char* data, std::size_t byteCount )
	{
		return SysWrite( data, byteCount );
	}
	
}

