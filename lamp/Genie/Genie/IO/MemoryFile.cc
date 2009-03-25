/*	=============
 *	MemoryFile.cc
 *	=============
 */

#include "Genie/IO/MemoryFile.hh"

// Nucleus
#include "Nucleus/NAssert.h"

// Io
#include "io/io.hh"


namespace Genie
{
	
	MemoryFileHandle::~MemoryFileHandle()
	{
	}
	
	boost::shared_ptr< IOHandle > MemoryFileHandle::Clone()
	{
		return boost::shared_ptr< IOHandle >( new MemoryFileHandle( GetFile(),
		                                                            GetFlags(),
		                                                            itsBase,
		                                                            itsSize ) );
	}
	
	ssize_t MemoryFileHandle::SysRead( char* data, std::size_t byteCount )
	{
		ASSERT( GetFileMark() <= itsSize );
		
		byteCount = std::min( byteCount, itsSize - GetFileMark() );
		
		std::copy( itsBase + GetFileMark(),
		           itsBase + GetFileMark() + byteCount,
		           data );
		
		return Advance( byteCount );
	}
	
	ssize_t MemoryFileHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		std::copy( data,
		           data + byteCount,
		           itsBase + GetFileMark() );
		
		return Advance( byteCount );
	}
	
}

