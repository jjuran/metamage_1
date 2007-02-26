/*	==============
 *	RegularFile.cc
 *	==============
 */

#include "Genie/IO/RegularFile.hh"


namespace Genie
{
	
	off_t RegularFileHandle::GetEOF() const
	{
		return GetFile()->GetEOF();
	}
	
	void RegularFileHandle::SetEOF( off_t length ) const
	{
		GetFile()->SetEOF( length );
	}
	
}

