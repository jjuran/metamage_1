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
			int its_ppid;
			
			boost::intrusive_ptr< process_group > its_process_group;
		
		public:
			process( int id, int ppid, process_group& pg );
			
			~process();
			
			int id() const  { return its_id; }
			
			int getppid() const  { return its_ppid; }
			
			void orphan()  { its_ppid = 1; }
			
			void clear_ppid()  { its_ppid = 0; }
			
			process_group& get_process_group() const;
			
			void set_process_group( process_group& pg );
	};
	
}

#endif

