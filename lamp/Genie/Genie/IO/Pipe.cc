/*	=======
 *	File.cc
 *	=======
 */

#include "Genie/IO/Pipe.hh"

// Standard C++
#include <algorithm>

// POSeven
#include "POSeven/Errno.hh"

// Io
#include "Io/Exceptions.hh"

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
		
		if ( partialRead.empty() )
		{
			if ( myStrings.empty() )
			{
				if ( !inputClosed )
				{
					if ( !isBlocking )
					{
						throw Io::NoDataPending();
					}
					
					while ( myStrings.empty() && !inputClosed )
					{
						Yield();
					}
				}
				
				// Either a string was written, or input was closed,
				// or possibly both, so check myStrings rather than inputClosed
				// so we don't miss data.
				
				if ( myStrings.empty() )
				{
					throw Io::EndOfInput();
				}
			}
			
			// Only reached if a string is available.
			partialRead = myStrings.front();
			myStrings.pop_front();
		}
		
		std::size_t bytesCopied = std::min( partialRead.size(), byteCount );
		
		std::copy( partialRead.begin(),
		           partialRead.begin() + bytesCopied,
		           data );
		
		std::copy( partialRead.begin() + bytesCopied,
		           partialRead.end(),
		           partialRead.begin() );
		
		partialRead.resize( partialRead.size() - bytesCopied );
		
		return bytesCopied;
	}
	
	int PipeState::Write( const char* data, std::size_t byteCount )
	{
		if ( byteCount != 0 )
		{
			myStrings.push_back( std::string( data, byteCount ) );
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

