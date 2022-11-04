/*
	get_session_process_group.hh
	----------------------------
*/

#ifndef RELIX_API_GETSESSIONPROCESSGROUP_HH
#define RELIX_API_GETSESSIONPROCESSGROUP_HH

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
	get_session_process_group( pid_t pgid, session& s );
	
}

#endif
