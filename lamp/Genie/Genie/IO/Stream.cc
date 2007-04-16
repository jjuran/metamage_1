/*	=========
 *	Stream.cc
 *	=========
 */

#include "Genie/IO/Stream.hh"

// Standard C++
#include <algorithm>

// Nucleus
#include "Nucleus/NAssert.h"

// POSeven
#include "POSeven/Errno.hh"


namespace Genie
{
	
	namespace P7 = POSeven;
	
	// Return a reference to the peek buffer with at least minBytes of data in it.
	// We Read() until we have enough data.  If there're not enough data available,
	// then we either block, or throw EWOULDBLOCK if the stream is non-blocking.
	
	const std::string& StreamHandle::Peek( ByteCount minBytes )
	{
		while ( peekBuffer.size() < minBytes )
		{
			const ByteCount kBufferLength = 512;
			
			char data[ kBufferLength ];
			
			int bytes = SysRead( data, kBufferLength );  // will block, throw, or return positive
			
			ASSERT( bytes > 0 );
			
			peekBuffer.append( data, bytes );
		}
		
		return peekBuffer;
	}
	
	unsigned int StreamHandle::Poll() const
	{
		return SysPoll() | (peekBuffer.empty() ? 0 : kPollRead);
	}
	
	int StreamHandle::Read( char* data, std::size_t byteCount )
	{
		ByteCount bytesRead = std::min( peekBuffer.size(), byteCount );
		
		if ( bytesRead > 0 )
		{
			std::copy( peekBuffer.begin(),
			           peekBuffer.begin() + bytesRead,
			           data );
			
			std::copy( peekBuffer.begin() + bytesRead,
			           peekBuffer.end(),
			           peekBuffer.begin() );
			
			peekBuffer.resize( peekBuffer.size() - bytesRead );
			
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
	
	int StreamHandle::Write( const char* data, std::size_t byteCount )
	{
		return SysWrite( data, byteCount );
	}
	
	void StreamHandle::IOCtl( unsigned long /*request*/, int* /*argp*/ )
	{
		P7::ThrowErrno( EINVAL );
	}
	
}

