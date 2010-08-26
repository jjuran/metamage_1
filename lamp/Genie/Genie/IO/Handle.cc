/*
	Handle.cc
	---------
*/

#include "Genie/IO/Handle.hh"

// Nitrogen
#include "Nitrogen/MacMemory.hh"

// poseven
#include "poseven/types/errno_t.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	Handle_IOHandle::~Handle_IOHandle()
	{
	}
	
	boost::shared_ptr< IOHandle > Handle_IOHandle::Clone()
	{
		return seize_ptr( new Handle_IOHandle( GetFile(),
		                                       GetFlags(),
		                                       itsHandle ) );
	}
	
	ssize_t Handle_IOHandle::Positioned_Read( char* buffer, size_t n_bytes, off_t offset )
	{
		const size_t size = GetEOF();
		
		if ( offset >= size )
		{
			return 0;
		}
		
		n_bytes = std::min< size_t >( n_bytes, size - offset );
		
		memcpy( buffer, *itsHandle.get().Get() + offset, n_bytes );
		
		return n_bytes;
	}
	
	ssize_t Handle_IOHandle::Positioned_Write( const char* buffer, size_t n_bytes, off_t offset )
	{
		const bool writable = GetFlags() + (1 - O_RDONLY) & 2;
		
		if ( !writable )
		{
			p7::throw_errno( EPERM );
		}
		
		if ( n_bytes == 0 )
		{
			return 0;
		}
		
		const size_t required_size = offset + n_bytes;
		
		const size_t existing_size = GetEOF();
		
		if ( required_size > existing_size )
		{
			try
			{
				N::SetHandleSize( itsHandle, required_size );
			}
			catch ( ... )
			{
				if ( offset > existing_size )
				{
					throw;
				}
				
				n_bytes = std::min< size_t >( n_bytes, existing_size - offset );
			}
		}
		
		memcpy( *itsHandle.get().Get() + offset, buffer, n_bytes );
		
		return n_bytes;
	}
	
	off_t Handle_IOHandle::GetEOF()
	{
		return N::GetHandleSize( itsHandle );
	}
	
	void Handle_IOHandle::SetEOF( off_t length )
	{
		N::SetHandleSize( itsHandle, length );
	}
	
}

