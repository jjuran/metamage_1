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

// relix-kernel
#include "relix/task/alarm_clock.hh"


namespace relix
{
	
	class process_group;
	
	
	class process : public plus::ref_count< process >
	{
		private:
			int its_id;
			int its_ppid;
			
			alarm_clock its_alarm_clock;
			
			boost::intrusive_ptr< process_group > its_process_group;
		
		public:
			process( int id, int ppid, process_group& pg );
			
			~process();
			
			int id() const  { return its_id; }
			
			int getppid() const  { return its_ppid; }
			
			void orphan()  { its_ppid = 1; }
			
			void clear_ppid()  { its_ppid = 0; }
			
			alarm_clock& get_alarm_clock()  { return its_alarm_clock; }
			
			process_group& get_process_group() const;
			
			void set_process_group( process_group& pg );
	};
	
}

#endif

