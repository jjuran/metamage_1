/*	================
 *	DynamicGroups.cc
 *	================
 */

#include "Genie/FileSystem/DynamicGroups.hh"

// Nucleus
#include "Nucleus/NAssert.h"

// POSeven
#include "POSeven/Pathnames.hh"


namespace Genie
{
	
	std::string DynamicGroup_Details_Base::GetChildName( const Sequence::value_type& child )
	{
		ASSERT( !child.second.expired() );
		
		boost::shared_ptr< IOHandle > io = child.second.lock();
		
		return Nucleus::Convert< std::string >( child.first );
	}
	
}

