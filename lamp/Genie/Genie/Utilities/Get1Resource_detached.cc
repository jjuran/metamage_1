/*
	Get1Resource_detached.cc
	------------------------
*/

#include "Genie/Utilities/Get1Resource_detached.hh"

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// Genie
#include "Genie/Utilities/checked_resource.hh"


namespace Genie
{

namespace n = nucleus;

n::owned< Mac::Handle > Get1Resource_detached( ResType type, short id )
{
	Handle h = checked_resource( Get1Resource( type, id ) );
	
	DetachResource( h );
	
	return n::owned< Mac::Handle >::seize( h );
}

}
