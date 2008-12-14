/*	==============
 *	RegularFile.cc
 *	==============
 */

#include "Genie/IO/RegularFile.hh"

// POSIX
#include <fcntl.h>

// POSeven
#include "POSeven/Errno.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	RegularFileHandle::RegularFileHandle( OpenFlags flags ) : FileHandle( flags ), itsMark()
	{
	}
	
	RegularFileHandle::~RegularFileHandle()
	{
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
	
	off_t RegularFileHandle::GetEOF() const
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
	
}

