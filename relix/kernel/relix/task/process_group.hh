/*
	process_group.hh
	----------------
*/

#ifndef RELIX_TASK_PROCESSGROUP_HH
#define RELIX_TASK_PROCESSGROUP_HH

// Debug
#include "debug/boost_assert.hh"

// vxo
#include "vxo/ref_count.hh"

// Boost
#include <boost/intrusive_ptr.hpp>


namespace relix
{
	
	class session;
	
	
	const int no_pgid = 0x7fffffff;
	
	class process_group : public vxo::ref_count< process_group >
	{
		private:
			int its_id;
			
			boost::intrusive_ptr< session > its_session;
		
		public:
			process_group( int id );
			
			process_group( int id, session& s );
			
			~process_group();
			
			int id() const  { return its_id; }
			
			int getsid() const;
			
			session& get_session() const;
	};
	
}

#endif
