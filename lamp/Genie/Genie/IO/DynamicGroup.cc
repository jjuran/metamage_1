/*	===============
 *	DynamicGroup.cc
 *	===============
 */

#include "Genie/IO/DynamicGroup.hh"

// Standard C
#include "errno.h"

// Debug
#include "debug/assert.hh"

// poseven
#include "poseven/types/errno_t.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	boost::shared_ptr< IOHandle > GetDynamicElementFromGroupByID( const DynamicGroup&  group,
	                                                              DynamicElementID     id )
	{
		DynamicGroup::const_iterator it = group.find( id );
		
		if ( it == group.end() )
		{
			p7::throw_errno( ENOENT );
		}
		
		ASSERT( !it->second.expired() );
		
		return it->second.lock();
	}
	
}

