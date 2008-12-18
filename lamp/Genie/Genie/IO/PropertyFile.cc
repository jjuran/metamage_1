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
	
	
	ssize_t PropertyReaderFileHandle::SysRead( char* buffer, std::size_t byteCount )
	{
		ASSERT( GetFileMark() <= itsData.size() );
		
		byteCount = std::min( byteCount, itsData.size() - GetFileMark() );
		
		std::copy( itsData.begin() + GetFileMark(),
		           itsData.begin() + GetFileMark() + byteCount,
		           buffer );
		
		return Advance( byteCount );
	}
	
	
	ssize_t PropertyWriterFileHandle::SysWrite( const char* buffer, std::size_t byteCount )
	{
		if ( byteCount == 0 )
		{
			throw p7::errno_t( EINVAL );
		}
		
		std::size_t length = byteCount - 1;
		
		if ( buffer[ length ] != '\n' )
		{
			throw p7::errno_t( EINVAL );
		}
		
		itsWriteHook( GetKey(), buffer, buffer + length );
		
		return byteCount;
	}
	
}

