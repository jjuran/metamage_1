/*	=========
 *	Stream.cc
 *	=========
 */

#include "Genie/IO/Stream.hh"

// Standard C++
#include <algorithm>

// POSIX
#include "sys/ioctl.h"

// Nucleus
#include "Nucleus/NAssert.h"

// POSeven
#include "POSeven/Errno.hh"

// Genie
#include "Genie/Process.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	StreamHandle::~StreamHandle()
	{
	}
	
	void StreamHandle::TryAgainLater() const
	{
		Genie::TryAgainLater( IsBlocking() );
	}
	
	// Return a reference to the peek buffer with at least minBytes of data in it.
	// We Read() until we have enough data.  If there're not enough data available,
	// then we either block, or throw EWOULDBLOCK if the stream is non-blocking.
	
	const std::string& StreamHandle::Peek( ByteCount minBytes )
	{
		if ( IsDisconnected() )
		{
			p7::throw_errno( EIO );
		}
		
		while ( peekBuffer.size() < minBytes )
		{
			const ByteCount kBufferLength = 512;
			
			char data[ kBufferLength ];
			
			int bytes = SysRead( data, kBufferLength );  // will block, throw, or return non-negative
			
			if ( bytes == 0 )
			{
				throw io::end_of_input();
			}
			
			peekBuffer.append( data, bytes );
		}
		
		return peekBuffer;
	}
	
	unsigned int StreamHandle::Poll() const
	{
		if ( IsDisconnected() )
		{
			p7::throw_errno( EIO );
		}
		
		return SysPoll() | (peekBuffer.empty() ? 0 : kPollRead);
	}
	
	int StreamHandle::Read( char* data, std::size_t byteCount )
	{
		if ( IsDisconnected() )
		{
			p7::throw_errno( EIO );
		}
		
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
		if ( IsDisconnected() )
		{
			p7::throw_errno( EIO );
		}
		
		return SysWrite( data, byteCount );
	}
	
	void StreamHandle::IOCtl( unsigned long request, int* argp )
	{
		switch ( request )
		{
			case FIONBIO:
				if ( *argp )
				{
					SetNonBlocking();
				}
				else
				{
					SetBlocking();
				}
				
				break;
			
			case FIONREAD:
				// not implemented
				return p7::throw_errno( EINVAL );
			
			default:
				IOHandle::IOCtl( request, argp );
				break;
		}
	}
	
}

