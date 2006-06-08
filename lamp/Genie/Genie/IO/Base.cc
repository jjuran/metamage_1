/*	=======
 *	Base.cc
 *	=======
 */

#include "Genie/IO/Base.hh"

// POSIX
#include "sys/errno.h"

// POSeven
#include "POSeven/Errno.hh"


namespace Genie
{
	
	namespace P7 = POSeven;
	
	void Check_IOHandle_Cast( IOHandle& handle, TypeCode desiredType )
	{
		TypeCode actual = handle.ActualType();
		
		if ( actual % desiredType != 0 )
		{
			P7::ThrowErrno( EOPNOTSUPP );
		}
	}
	
}

