/*	========
 *	IORef.cc
 *	========
 */

#include "Genie/IORef.hh"

// Nitrogen Core
#include "Nitrogen/Assert.h"


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
	
	/*
	const std::string& IORef::Peek( ByteCount minBytes ) const
	{
		ASSERT( fStream != NULL );
		
		return fStream->Peek( minBytes );
	}
	*/
	
	const std::string& IORef::Peek( ByteCount minBytes ) const
	{
		if ( fStream == NULL )  return std::string();
		
		return fStream->Peek( minBytes );
	}
	
	unsigned int IORef::Poll() const
	{
		if ( fStream == NULL )  return 0;
		
		return fStream->Poll();
	}
	
	int IORef::Read( char* data, std::size_t byteCount ) const
	{
		if ( fStream == NULL )  return 0;
		
		return fStream->Read( data, byteCount );
	}
	
	int IORef::Write( const char* data, std::size_t byteCount ) const
	{
		if ( fStream == NULL )  return 0;
		
		return fStream->Write( data, byteCount );
	}
	
	typedef IOReader Reader;
	typedef IOWriter Writer;
	typedef IOPoller Poller;
	
	struct IOTypeRecord
	{
		RefMod refMod;
		Reader reader;
		Writer writer;
		Poller poller;
		
		IOTypeRecord()
		:
			refMod( NULL ),
			reader( NULL ),
			writer( NULL ),
			poller( NULL )
		{}
		
		IOTypeRecord( RefMod refMod, Reader reader, Writer writer, Poller poller )
		:
			refMod( refMod ),
			reader( reader ),
			writer( writer ),
			poller( poller )
		{}
	};
	
	typedef std::vector< IOTypeRecord > IOTypeVector;
	
	static IOTypeVector& IOTypes()
	{
		static IOTypeVector gIOTypes;
		
		return gIOTypes;
	}
	
	
	void RetainIO( std::size_t type, std::size_t offset )
	{
		if ( type != 0 )
		{
			IOTypes().at( type ).refMod( offset, +1 );
		}
	}
	
	void ReleaseIO( std::size_t type, std::size_t offset )
	{
		if ( type != 0 )
		{
			IOTypes().at( type ).refMod( offset, -1 );
		}
	}
	
	unsigned int PollIO( std::size_t type, std::size_t offset )
	{
		return IOTypes().at( type ).poller( offset );
	}
	
	int ReadIO( std::size_t type, std::size_t offset, char* data, std::size_t byteCount )
	{
		return IOTypes().at( type ).reader( offset, data, byteCount );
	}
	
	int WriteIO( std::size_t type, std::size_t offset, const char* data, std::size_t byteCount )
	{
		return IOTypes().at( type ).writer( offset, data, byteCount );
	}
	
	void RegisterIOType( std::size_t type, RefMod refMod, Reader reader, Writer writer, IOPoller poller )
	{
		if ( IOTypes().size() <= type )
		{
			IOTypes().resize( type + 1 );
		}
		
		IOTypes()[ type ] = IOTypeRecord( refMod, reader, writer, poller );
	}
	
}

