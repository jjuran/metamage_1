/*	==============
 *	RegularFile.cc
 *	==============
 */

#include "Genie/IO/RegularFile.hh"


namespace Genie
{
	
	RegularFileHandle::~RegularFileHandle()
	{
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

