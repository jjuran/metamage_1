/*	==============
 *	RegularFile.cc
 *	==============
 */

#include "Genie/IO/RegularFile.hh"

// POSIX
#include <fcntl.h>

// Nitrogen
#include "Nitrogen/MacMemory.h"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/mmap/Handle_memory_mapping.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	
	class malloc_memory_mapping : public memory_mapping
	{
		private:
			void* its_address;
			
			addr_t get_address() const  { return its_address; }
		
		private:
			// non-copyable
			malloc_memory_mapping           ( const malloc_memory_mapping& );
			malloc_memory_mapping& operator=( const malloc_memory_mapping& );
		
		public:
			malloc_memory_mapping( void* addr ) : its_address( addr )
			{
			}
			
			~malloc_memory_mapping()
			{
				free( its_address );
			}
	};
	
	
	RegularFileHandle::RegularFileHandle( OpenFlags flags ) : FileHandle( flags ), itsMark()
	{
	}
	
	RegularFileHandle::~RegularFileHandle()
	{
	}
	
	ssize_t RegularFileHandle::Positioned_Read( char* buffer, size_t n_bytes, off_t offset )
	{
		p7::throw_errno( EPERM );
		
		return 0;
	}
	
	ssize_t RegularFileHandle::Positioned_Write( const char* buffer, size_t n_bytes, off_t offset )
	{
		p7::throw_errno( EPERM );
		
		return 0;
	}
	
	ssize_t RegularFileHandle::Append( const char* buffer, size_t n_bytes )
	{
		itsMark = GetEOF();
		
		return Positioned_Write( buffer, n_bytes, itsMark );
	}
	
	ssize_t RegularFileHandle::SysRead( char* buffer, size_t n_bytes )
	{
		ssize_t read = Positioned_Read( buffer,
		                                n_bytes,
		                                GetFileMark() );
		
		return Advance( read );
	}
	
	ssize_t RegularFileHandle::SysWrite( const char* buffer, size_t n_bytes )
	{
		const bool appending = GetFlags() & O_APPEND;
		
		ssize_t written = appending ? Append( buffer, n_bytes )
		                            : Positioned_Write( buffer,
		                                                n_bytes,
		                                                GetFileMark() );
		
		return Advance( written );
	}
	
	off_t RegularFileHandle::Seek( off_t offset, int whence )
	{
		off_t base = 0;
		
		switch ( whence )
		{
			case SEEK_SET:
				base = 0;
				break;
			
			case SEEK_CUR:
				base = GetFileMark();
				break;
			
			case SEEK_END:
				base = GetEOF();
				break;
			
			default:
				p7::throw_errno( EINVAL );
		}
		
		itsMark = base + offset;
		
		return itsMark;
	}
	
	off_t RegularFileHandle::GetEOF()
	{
		return GetFile()->GetEOF();
	}
	
	void RegularFileHandle::SetEOF( off_t length )
	{
		GetFile()->SetEOF( length );
	}
	
	ssize_t RegularFileHandle::Write( const char* buffer, std::size_t byteCount )
	{
		OpenFlags flags = GetFlags();
		
		if ( flags & O_TRUNC_LAZY )
		{
			SetEOF( GetFileMark() );
			
			SetFlags( flags & ~O_LAZY );
		}
		
		return SysWrite( buffer, byteCount );
	}
	
	memory_mapping::shared_ptr RegularFileHandle::Map( size_t length, off_t offset )
	{
		memory_mapping* mapping = NULL;
		
		if ( const bool small = length < 64 * 1024 )
		{
			if ( void* addr = malloc( length ) )
			{
				mapping = new malloc_memory_mapping( addr );
			}
		}
		
		if ( mapping == NULL )
		{
			// Either malloc() failed or we didn't try.
			
			NN::Owned< N::Handle > h = N::TempNewHandle( length );
			
			mapping = new Handle_memory_mapping( h );
		}
		
		memory_mapping::shared_ptr result( mapping );
		
		void* addr = mapping->get_address();
		
		ssize_t count = Positioned_Read( (char*) addr, length, offset );
		
		if ( count < length )
		{
			memset( (char*) addr + count, '\0', length - count );
		}
		
		return result;
	}
	
}

