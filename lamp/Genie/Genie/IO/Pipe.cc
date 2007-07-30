/*	=======
 *	File.cc
 *	=======
 */

#include "Genie/IO/Pipe.hh"

// Standard C++
#include <algorithm>

// Io
#include "io/io.hh"

// POSeven
#include "POSeven/Errno.hh"

// Genie
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace P7 = POSeven;
	
	int PipeState::Read( char* data, std::size_t byteCount )
	{
		if ( byteCount == 0 )
		{
			return 0;
		}
		
		if ( itsStrings.empty() )
		{
			// Oops, nothing queued right now
			
			if ( !itsInputHasClosed )
			{
				// Still open, maybe we'll get something later
				
				if ( !itIsBlocking )
				{
					// But we're not going to wait
					throw io::no_input_pending();
				}
				
				// Input or bust
				while ( itsStrings.empty() && !itsInputHasClosed )
				{
					// I can wait forever...
					Yield();
				}
			}
			
			// Either a string was written, or input was closed,
			// or possibly both, so check itsStrings rather than itsInputHasClosed
			// so we don't miss data.
			
			// If the string queue is still empty then input must have closed.
			if ( itsStrings.empty() )
			{
				throw io::end_of_input();
			}
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
	
	int PipeState::Write( const char* data, std::size_t byteCount )
	{
		if ( byteCount != 0 )
		{
			itsStrings.push_back( std::string( data, byteCount ) );
		}
		
		return byteCount;
	}
	
	
	PipeInHandle::~PipeInHandle()
	{
		state->CloseInput();
	}
	
	int PipeInHandle::SysRead( char* /*data*/, std::size_t /*byteCount*/ )
	{
		P7::ThrowErrno( EPERM );
		
		return -1;
	}
	
	
	PipeOutHandle::~PipeOutHandle()
	{
		state->CloseOutput();
	}
	
	int PipeOutHandle::SysWrite( const char* /*data*/, std::size_t /*byteCount*/ )
	{
		P7::ThrowErrno( EPERM );
		
		return -1;
	}
	
}

