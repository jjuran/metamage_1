/*	=============
 *	MemoryFile.cc
 *	=============
 */

#include "Genie/IO/MemoryFile.hh"

// Nucleus
#include "Nucleus/NAssert.h"

// Io
#include "io/io.hh"

// POSeven
#include "POSeven/Errno.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	MemoryFileHandle::~MemoryFileHandle()
	{
	}
	
	int MemoryFileHandle::SysRead( char* data, std::size_t byteCount )
	{
		ASSERT( itsMark <= itsSize );
		
		byteCount = std::min( byteCount, itsSize - itsMark );
		
		std::copy( itsBase + itsMark,
		           itsBase + itsMark + byteCount,
		           data );
		
		itsMark += byteCount;
		
		return byteCount;
	}
	
	int MemoryFileHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		std::copy( data,
		           data + byteCount,
		           itsBase + itsMark );
		
		itsMark += byteCount;
		
		return byteCount;
	}
	
	off_t MemoryFileHandle::Seek( off_t offset, int whence )
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
				itsMark = itsSize + offset;
				break;
			
			default:
				p7::throw_errno( EINVAL );
		}
		
		return itsMark;
	}
	
}

