/*
	get_process_group_in_session.hh
	-------------------------------
*/

#ifndef RELIX_API_GETPROCESSGROUPINSESSION_HH
#define RELIX_API_GETPROCESSGROUPINSESSION_HH

// Debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/intrusive_ptr.hpp>


namespace relix
{
	
	class process_group;
	class session;
	
	boost::intrusive_ptr< process_group >
	//
	get_process_group_in_session( pid_t pgid, session& s );
	
}

#endif
