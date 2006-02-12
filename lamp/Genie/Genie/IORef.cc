/*	========
 *	IORef.cc
 *	========
 */

#include "Genie/IORef.hh"

// Nucleus
#include "Nucleus/NAssert.h"


namespace Genie
{
	
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
	
	bool IORef::IsBlocking() const
	{
		if ( fStream == NULL )  return true;
		
		return fStream->IsBlocking();
	}
	
	void IORef::SetBlocking()
	{
		if ( fStream == NULL )  return;
		
		fStream->SetBlocking();
	}
	
	void IORef::SetNonBlocking()
	{
		if ( fStream == NULL )  return;
		
		fStream->SetNonBlocking();
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

