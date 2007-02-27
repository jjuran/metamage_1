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
		
		if ( itsAvailableInput.empty() )
		{
			// Need more input.
			
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
			itsAvailableInput = itsStrings.front();
			itsStrings.pop_front();
		}
		
		std::size_t bytesCopied = std::min( itsAvailableInput.size(), byteCount );
		
		// Copy from our input store to the supplied buffer
		std::copy( itsAvailableInput.begin(),
		           itsAvailableInput.begin() + bytesCopied,
		           data );
		
		// Slide any unused input to the beginning
		// This would really suck for reading lots of data through a pipe one byte at a time
		std::copy( itsAvailableInput.begin() + bytesCopied,
		           itsAvailableInput.end(),
		           itsAvailableInput.begin() );
		
		itsAvailableInput.resize( itsAvailableInput.size() - bytesCopied );
		
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
	
	int PipeInHandle::SysRead( char* data, std::size_t byteCount )
	{
		P7::ThrowErrno( EPERM );
		
		return -1;
	}
	
	int PipeOutHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		P7::ThrowErrno( EPERM );
		
		return -1;
	}
	
}

