/*	===============
 *	ProcessGroup.cc
 *	===============
 */

#include "Genie/ProcessGroup.hh"


namespace Genie
{
	
	boost::intrusive_ptr< relix::session > NewSession( pid_t sid )
	{
		return new relix::session( sid );
	}
	
}

