/*	==========
 *	Conduit.cc
 *	==========
 */

#include "Genie/IO/Conduit.hh"

// Standard C
#include <errno.h>
#include <signal.h>

// Standard C++
#include <algorithm>

// Io
#include "io/io.hh"

// POSeven
#include "POSeven/Errno.hh"

// Genie
#include "Genie/Process.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	bool Conduit::IsReadable() const
	{
		return itsIngressHasClosed || !itsStrings.empty();
	}
	
	bool Conduit::IsWritable() const
	{
		return itsEgressHasClosed || itsStrings.size() < 20;
	}
	
	int Conduit::Read( char* data, std::size_t byteCount, bool nonblocking )
	{
		if ( byteCount == 0 )
		{
			return 0;
		}
		
		// Wait until we have some data or the stream is closed
		while ( itsStrings.empty() && !itsIngressHasClosed )
		{
			TryAgainLater( nonblocking );
		}
		
		// Either a string was written, or input was closed,
		// or possibly both, so check itsStrings rather than itsIngressHasClosed
		// so we don't miss data.
		
		// If the string queue is still empty then input must have closed.
		if ( itsStrings.empty() )
		{
			return 0;
		}
		
		// Only reached if a string is available.
		std::size_t bytesCopied = 0;
		
		do
		{
			std::string& inputChunk = itsStrings.front();
			//itsStrings.pop_front();
			
			std::size_t bytesToCopy = std::min( inputChunk.size(), byteCount - bytesCopied );
			
			// Copy from our input store to the supplied buffer
			std::copy( inputChunk.begin(),
			           inputChunk.begin() + bytesToCopy,
			           data + bytesCopied );
			
			// Slide any unused input to the beginning
			// This would really suck for reading lots of data through a pipe one byte at a time
			std::copy( inputChunk.begin() + bytesToCopy,
			           inputChunk.end(),
			           inputChunk.begin() );
			
			bytesCopied += bytesToCopy;
			
			inputChunk.resize( inputChunk.size() - bytesToCopy );
			
			if ( inputChunk.empty() )
			{
				itsStrings.pop_front();
			}
		}
		while ( !itsStrings.empty()  &&  bytesCopied < byteCount );
		
		return bytesCopied;
	}
	
	int Conduit::Write( const char* data, std::size_t byteCount, bool nonblocking )
	{
		while ( !IsWritable() )
		{
			TryAgainLater( nonblocking );
		}
		
		if ( itsEgressHasClosed )
		{
			Process& current = CurrentProcess();
			
			current.Raise( SIGPIPE );
			
			current.HandlePendingSignals();
			
			p7::throw_errno( EPIPE );
		}
		
		if ( byteCount != 0 )
		{
			itsStrings.push_back( std::string( data, byteCount ) );
		}
		
		return byteCount;
	}
	
}

