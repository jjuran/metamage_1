/*	===============
 *	PropertyFile.cc
 *	===============
 */

#include "Genie/IO/PropertyFile.hh"

// POSeven
#include "POSeven/Errno.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	boost::shared_ptr< IOHandle > PropertyReaderFileHandle::Clone()
	{
		return boost::shared_ptr< IOHandle >( new PropertyReaderFileHandle( GetFile(), GetFlags(), itsData ) );
	}
	
	ssize_t PropertyReaderFileHandle::Positioned_Read( char* buffer, size_t n_bytes, off_t offset )
	{
		if ( offset >= itsData.size() )
		{
			return 0;
		}
		
		n_bytes = std::min( n_bytes, itsData.size() - offset );
		
		memcpy( buffer, itsData.begin() + offset, n_bytes );
		
		return n_bytes;
	}
	
	
	boost::shared_ptr< IOHandle > PropertyWriterFileHandle::Clone()
	{
		return boost::shared_ptr< IOHandle >( new PropertyWriterFileHandle( GetFile(),
		                                                                    GetFlags(),
		                                                                    itsWriteHook,
		                                                                    itIsBinary ) );
	}
	
	ssize_t PropertyWriterFileHandle::SysWrite( const char* buffer, std::size_t byteCount )
	{
		if ( byteCount == 0 )
		{
			throw p7::errno_t( EINVAL );
		}
		
		std::size_t length = byteCount;
		
		if ( !itIsBinary  &&  buffer[ --length ] != '\n' )
		{
			throw p7::errno_t( EINVAL );
		}
		
		itsWriteHook( GetFile().get(), buffer, buffer + length, itIsBinary );
		
		return byteCount;
	}
	
}

