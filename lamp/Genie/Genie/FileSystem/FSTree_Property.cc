/*	==================
 *	FSTree_Property.cc
 *	==================
 */

#include "Genie/FileSystem/FSTree_Property.hh"

// POSIX
#include <fcntl.h>

// POSeven
#include "POSeven/Errno.hh"

// Genie
#include "Genie/IO/PropertyFile.hh"


namespace Genie
{
	
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	
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
	
}

