/*	========
 *	Pipes.cc
 *	========
 */

#include "Genie/Pipes.hh"

// Standard C++
#include <algorithm>

// Io
#include "Io/Exceptions.hh"

// Genie
#include "Genie/Yield.hh"


namespace Genie
{
	
	Pipe::Handles Pipe::New()
	{
		N::Shared< PipeState* > pipe = N::Owned< PipeState* >::Seize( new PipeState );
		
		return Handles( PipeInHandle ( pipe, N::Owned< PipeState*, CloseInput  >::Seize( pipe.Get() ) ),
		                PipeOutHandle( pipe, N::Owned< PipeState*, CloseOutput >::Seize( pipe.Get() ) ) );
	}
	
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
					if ( !fBlocking )
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
	
	int PipeInHandle::Read( char* data, std::size_t byteCount ) const
	{
		throw NotReadable();
		return -1;  // Not reached
	}
	
	int PipeInHandle::Write( const char* data, std::size_t byteCount ) const
	{
		return pipe.Get()->Write( data, byteCount );
	}
	
	int PipeOutHandle::Read( char* data, std::size_t byteCount ) const
	{
		return pipe.Get()->Read( data, byteCount );
	}
	
	int PipeOutHandle::Write( const char* data, std::size_t byteCount ) const
	{
		throw NotWriteable();
		return -1;  // Not reached
	}
	
}

