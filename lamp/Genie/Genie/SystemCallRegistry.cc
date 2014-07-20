/*	=====================
 *	SystemCallRegistry.cc
 *	=====================
 */

#include "Genie/SystemCallRegistry.hh"

// Standard C
#include <string.h>


namespace Genie
{
	
	using namespace relix;
	
	
	const SystemCall* LookUpSystemCallByName( const char* name )
	{
		SystemCall* it  = the_syscall_array;
		SystemCall* end = it + the_last_syscall;
		
		while ( it < end )
		{
			if ( strcmp( name, it->name ) == 0 )
			{
				return it;
			}
		}
		
		return NULL;
	}
	
}
