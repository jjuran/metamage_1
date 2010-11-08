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


#ifndef O_BINARY
#define O_BINARY  0
#endif


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
				
				plus::var_string data;
				
				itsReadHook( data, ParentRef().get(), binary_vs_text );
				
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
		
		plus::var_string data;
		
		try
		{
			const bool binary = true;  // Return binary length
			
			itsReadHook( data, ParentRef().get(), binary );
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
		
		plus::var_string data;
		
		try
		{
			const bool binary = flags & O_BINARY;
			
			itsReadHook( data, ParentRef().get(), binary );
			
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
	
	FSTreePtr new_property( const FSTreePtr&     parent,
	                        const plus::string&  name,
	                        const void*          params_ )
	{
		const property_params& params = *(const property_params*) params_;
		
		return seize_ptr( new FSTree_Property( parent,
		                                       name,
		                                       params.size,
		                                       params.get,
		                                       params.set ) );
	}
	
}

