/*	===============
 *	DynamicGroup.cc
 *	===============
 */

#include "Genie/IO/DynamicGroup.hh"

// Standard C
#include "errno.h"

// poseven
#include "poseven/types/errno_t.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	IOPtr GetDynamicElementFromGroupByID( const DynamicGroup&  group,
	                                      DynamicElementID     id )
	{
		DynamicGroup::const_iterator it = group.find( id );
		
		if ( it == group.end() )
		{
			p7::throw_errno( ENOENT );
		}
		
		return it->second->shared_from_this();
	}
	
}

