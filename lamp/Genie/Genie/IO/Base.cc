/*	=======
 *	Base.cc
 *	=======
 */

#include "Genie/IO/Base.hh"


namespace vfs
{
	
	filehandle* filehandle::GetBaseForCast( Test test )
	{
		if ( (this->*test)() )
		{
			return this;
		}
		
		if ( filehandle* next = Next() )
		{
			return next->GetBaseForCast( test );
		}
		
		return NULL;
	}
	
}

