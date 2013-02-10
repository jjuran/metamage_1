/*	===============
 *	ProcessGroup.hh
 *	===============
 */

#ifndef GENIE_PROCESSGROUP_HH
#define GENIE_PROCESSGROUP_HH

// Debug
#include "debug/boost_assert.hh"

// plus
#include "plus/ref_count.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// relix
#include "relix/task/session.hh"

// Genie
#include "Genie/IO/IOPtr.hh"


namespace relix
{
	
	const int no_pgid = 0x7fffffff;
	
	class process_group : public plus::ref_count< process_group >
	{
		private:
			int its_id;
			
			boost::intrusive_ptr< session > its_session;
		
		public:
			process_group( int id, session& s )
			:
				its_id     ( id ),
				its_session( &s )
			{
			}
			
			~process_group();
			
			int id() const  { return its_id; }
			
			int getsid() const  { return its_session->id(); }
			
			session& get_session() const  { return *its_session; }
	};
	
}

namespace Genie
{
	
	typedef relix::session Session;
	
	typedef relix::process_group ProcessGroup;
	
	boost::intrusive_ptr< Session > NewSession( pid_t sid );
	
}

#endif

