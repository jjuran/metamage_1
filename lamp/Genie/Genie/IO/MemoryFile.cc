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
	
	ssize_t MemoryFileHandle::Positioned_Read( char* buffer, size_t n_bytes, off_t offset )
	{
		if ( offset >= itsSize )
		{
			return 0;
		}
		
		n_bytes = std::min( n_bytes, itsSize - offset );
		
		memcpy( buffer, itsBase + offset, n_bytes );
		
		return n_bytes;
	}
	
	ssize_t MemoryFileHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		std::copy( data,
		           data + byteCount,
		           itsBase + GetFileMark() );
		
		return Advance( byteCount );
	}
	
}

