/*	=============
 *	MemoryFile.cc
 *	=============
 */

#include "Genie/IO/MemoryFile.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/mmap/static_memory_mapping.hh"


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
		
		n_bytes = std::min< size_t >( n_bytes, itsSize - offset );
		
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
		
		n_bytes = std::min< size_t >( n_bytes, itsSize - offset );
		
		memcpy( itsBase + offset, buffer, n_bytes );
		
		return n_bytes;
	}
	
	memory_mapping::shared_ptr MemoryFileHandle::Map( size_t length, off_t offset )
	{
		if ( offset + length > itsSize )
		{
			p7::throw_errno( ENXIO );
		}
		
		return seize_ptr( new static_memory_mapping( itsBase + offset ) );
	}
	
}

