/*	==============
 *	RegularFile.cc
 *	==============
 */

#include "Genie/IO/RegularFile.hh"

// POSeven
#include "POSeven/Errno.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	RegularFileHandle::RegularFileHandle() : itsMark()
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
		
		base += offset;
		
		SetFileMark( base );
		
		return base;
	}
	
	off_t RegularFileHandle::GetEOF() const
	{
		return GetFile()->GetEOF();
	}
	
	void RegularFileHandle::SetEOF( off_t length )
	{
		GetFile()->SetEOF( length );
	}
	
}

