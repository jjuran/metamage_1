/*
	IOHandle_Cast.cc
	----------------
*/

#include "Genie/IO/IOHandle_Cast.hh"

// poseven
#include "poseven/types/errno_t.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	void Check_IOHandle_Cast( const void* cast, int errorToThrow )
	{
		if ( cast == NULL )
		{
			p7::throw_errno( errorToThrow );
		}
	}
	
}

