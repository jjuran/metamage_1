/*	==================
 *	FSTree_Property.cc
 *	==================
 */

#include "Genie/FS/FSTree_Property.hh"

// POSIX
#include <fcntl.h>

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/IO/PropertyFile.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	bool FSTree_Property::Exists() const
	{
		if ( itsReadHook != NULL )
		{
			try
			{
				// Binary is probably more efficient, but only slightly so
				const bool binary_vs_text = true;
				
				(void) itsReadHook( this, binary_vs_text );
				
				return true;
			}
			catch ( ... )
			{
			}
		}
		
		return false;
	}
	
	off_t FSTree_Property::GetEOF() const
	{
		if ( itsSize != 0 )
		{
			return itsSize;
		}
		
		std::string data;
		
		try
		{
			const bool binary = true;  // Return binary length
			
			data = itsReadHook( this, binary );
		}
		catch ( const Undefined& )
		{
		}
		
		return data.size();
	}
	
	boost::shared_ptr< IOHandle > FSTree_Property::Open( OpenFlags flags ) const
	{
		IOHandle* result = NULL;
		
		if ( (flags & ~O_BINARY) == O_RDONLY )
		{
			result = OpenForRead( flags );
		}
		else if ( (flags & ~(O_CREAT | O_BINARY)) == (O_WRONLY | O_TRUNC) )
		{
			result = OpenForWrite( flags );
		}
		else
		{
			throw p7::errno_t( EINVAL );
		}
		
		return boost::shared_ptr< IOHandle >( result );
	}
	
	IOHandle* FSTree_Property::OpenForRead( OpenFlags flags ) const
	{
		if ( itsReadHook == NULL )
		{
			p7::throw_errno( EACCES );
		}
		
		std::string data;
		
		try
		{
			const bool binary = flags & O_BINARY;
			
			data = itsReadHook( this, binary );
			
			if ( !binary )
			{
				data += '\n';
			}
		}
		catch ( const Undefined& )
		{
		}
		
		return new PropertyReaderFileHandle( Self(),
		                                     flags,
		                                     data );
	}
	
	IOHandle* FSTree_Property::OpenForWrite( OpenFlags flags ) const
	{
		if ( itsWriteHook == NULL )
		{
			p7::throw_errno( EACCES );
		}
		
		const bool binary = flags & O_BINARY;
		
		return new PropertyWriterFileHandle( Self(),
		                                     flags,
		                                     itsWriteHook,
		                                     binary );
	}
	
	FSTreePtr New_FSTree_Property( const FSTreePtr&    parent,
	                               const std::string&  name,
	                               size_t              size,
	                               Property_ReadHook   readHook,
	                               Property_WriteHook  writeHook )
	{
		return FSTreePtr( new FSTree_Property( parent,
		                                       name,
		                                       size,
		                                       readHook,
		                                       writeHook ) );
	}
	
}

