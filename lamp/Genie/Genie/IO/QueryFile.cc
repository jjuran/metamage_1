/*	============
 *	QueryFile.cc
 *	============
 */

#include "Genie/IO/QueryFile.hh"

// Io
#include "io/io.hh"

// POSeven
#include "POSeven/Errno.hh"


namespace Genie
{
	
	namespace P7 = POSeven;
	
	int QueryFileHandle::SysRead( char* data, std::size_t byteCount )
	{
		if ( itsMark >= itsData.size() )
		{
			throw io::end_of_input();
		}
		
		byteCount = std::min( byteCount, itsData.size() - itsMark );
		
		std::copy( itsData.begin() + itsMark,
		           itsData.begin() + itsMark + byteCount,
		           data );
		
		itsMark += byteCount;
		
		return byteCount;
	}
	
	int QueryFileHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		if ( itsMark + byteCount > itsData.size() )
		{
			itsData.resize( itsMark + byteCount );
		}
		
		std::copy( data,
		           data + byteCount,
		           itsData.begin() + itsMark );
		
		itsMark += byteCount;
		
		return byteCount;
	}
	
	void QueryFileHandle::Stat( struct stat* sb ) const
	{
		//StatFile( file, sb );
	}
	
	off_t QueryFileHandle::Seek( off_t offset, int whence )
	{
		switch ( whence )
		{
			case SEEK_SET:
				itsMark = offset;
				break;
			
			case SEEK_CUR:
				itsMark += offset;
				break;
			
			case SEEK_END:
				itsMark = itsData.size() + offset;
				break;
			
			default:
				P7::ThrowErrno( EINVAL );
		}
		
		return itsMark;
	}
	
}

