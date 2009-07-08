/*	=============
 *	MemoryFile.cc
 *	=============
 */

#include "Genie/IO/MemoryFile.hh"

// Nucleus
#include "Nucleus/NAssert.h"

// POSeven
#include "POSeven/Errno.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
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
	
	ssize_t MemoryFileHandle::Positioned_Write( const char* buffer, size_t n_bytes, off_t offset )
	{
		if ( n_bytes == 0 )
		{
			return 0;
		}
		
		if ( offset >= itsSize )
		{
			p7::throw_errno( ENOSPC );
		}
		
		n_bytes = std::min( n_bytes, itsSize - offset );
		
		memcpy( itsBase + offset, buffer, n_bytes );
		
		return n_bytes;
	}
	
}

