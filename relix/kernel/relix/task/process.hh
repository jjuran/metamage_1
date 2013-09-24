/*
	process.hh
	----------
*/

#ifndef RELIX_TASK_PROCESS_HH
#define RELIX_TASK_PROCESS_HH

// Debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// plus
#include "plus/ref_count.hh"


namespace relix
{
	
	class process_group;
	
	
	class process : public plus::ref_count< process >
	{
		private:
			int its_id;
			
			boost::intrusive_ptr< process_group > its_process_group;
		
		public:
			process( int id, process_group& pg );
			
			~process();
			
			int id() const  { return its_id; }
			
			process_group& get_process_group() const;
			
			void set_process_group( process_group& pg );
	};
	
}

#endif

