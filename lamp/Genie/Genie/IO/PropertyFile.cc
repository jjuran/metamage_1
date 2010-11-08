/*	===============
 *	PropertyFile.cc
 *	===============
 */

#include "Genie/IO/PropertyFile.hh"

// poseven
#include "poseven/types/errno_t.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	boost::shared_ptr< IOHandle > PropertyReaderFileHandle::Clone()
	{
		return seize_ptr( new PropertyReaderFileHandle( GetFile(), GetFlags(), itsData ) );
	}
	
	ssize_t PropertyReaderFileHandle::Positioned_Read( char* buffer, size_t n_bytes, off_t offset )
	{
		if ( offset >= itsData.size() )
		{
			return 0;
		}
		
		n_bytes = std::min< size_t >( n_bytes, itsData.size() - offset );
		
		memcpy( buffer, &itsData[ offset ], n_bytes );
		
		return n_bytes;
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
		
		itsWriteHook( GetFile()->ParentRef().get(), buffer, buffer + length, itIsBinary );
		
		return byteCount;
	}
	
}

