/*	========
 *	IORef.cc
 *	========
 */

#include "Genie/IORef.hh"


namespace Genie
{
	
	typedef IOReader Reader;
	typedef IOWriter Writer;
	
	struct IOTypeRecord
	{
		RefMod refMod;
		Reader reader;
		Writer writer;
		
		IOTypeRecord()
		:
			refMod( NULL ),
			reader( NULL ),
			writer( NULL )
		{}
		
		IOTypeRecord( RefMod refMod, Reader reader, Writer writer )
		:
			refMod( refMod ),
			reader( reader ),
			writer( writer )
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
	
	int ReadIO( std::size_t type, std::size_t offset, char* data, std::size_t byteCount )
	{
		return IOTypes().at( type ).reader( offset, data, byteCount );
	}
	
	int WriteIO( std::size_t type, std::size_t offset, const char* data, std::size_t byteCount )
	{
		return IOTypes().at( type ).writer( offset, data, byteCount );
	}
	
	void RegisterIOType( std::size_t type, RefMod refMod, Reader reader, Writer writer )
	{
		if ( IOTypes().size() <= type )
		{
			IOTypes().resize( type + 1 );
		}
		
		IOTypes()[ type ] = IOTypeRecord( refMod, reader, writer );
	}
	
}

