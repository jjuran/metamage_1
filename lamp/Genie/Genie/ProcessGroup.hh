/*	===============
 *	ProcessGroup.hh
 *	===============
 */

#ifndef GENIE_PROCESSGROUP_HH
#define GENIE_PROCESSGROUP_HH

// Debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// relix
#include "relix/task/process_group.hh"
#include "relix/task/session.hh"

// Genie
#include "Genie/IO/IOPtr.hh"


namespace Genie
{
	
	typedef relix::process_group ProcessGroup;
	
	boost::intrusive_ptr< relix::session > NewSession( pid_t sid );
	
}

#endif

