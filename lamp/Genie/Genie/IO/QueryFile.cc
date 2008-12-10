/*	============
 *	QueryFile.cc
 *	============
 */

#include "Genie/IO/QueryFile.hh"

// Nucleus
#include "Nucleus/NAssert.h"

// Io
#include "io/io.hh"


namespace Genie
{
	
	QueryFileHandle::~QueryFileHandle()
	{
	}
	
	ssize_t QueryFileHandle::SysRead( char* data, std::size_t byteCount )
	{
		ASSERT( GetFileMark() <= itsData.size() );
		
		byteCount = std::min( byteCount, itsData.size() - GetFileMark() );
		
		std::copy( itsData.begin() + GetFileMark(),
		           itsData.begin() + GetFileMark() + byteCount,
		           data );
		
		return Advance( byteCount );
	}
	
	ssize_t QueryFileHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		if ( GetFileMark() + byteCount > itsData.size() )
		{
			itsData.resize( GetFileMark() + byteCount );
		}
		
		std::copy( data,
		           data + byteCount,
		           itsData.begin() + GetFileMark() );
		
		return Advance( byteCount );
	}
	
}

