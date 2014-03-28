/*
	process.hh
	----------
*/

#ifndef RELIX_TASK_PROCESS_HH
#define RELIX_TASK_PROCESS_HH

// POSIX
#include <time.h>
#include <sys/times.h>

// Debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// plus
#include "plus/ref_count.hh"
#include "plus/string_fwd.hh"

// relix-kernel
#include "relix/task/alarm_clock.hh"


namespace relix
{
	
	class process_group;
	class process_image;
	class process_resources;
	
	typedef process_resources process_rsrcs;
	
	
	class process : public plus::ref_count< process >
	{
		private:
			int its_id;
			int its_ppid;
			
			clock_t its_last_activity;
			
			struct tms its_times;
			
			alarm_clock its_alarm_clock;
			
			boost::intrusive_ptr< process_group > its_process_group;
			boost::intrusive_ptr< process_image > its_process_image;
			boost::intrusive_ptr< process_rsrcs > its_process_resources;
		
		public:
			process( int id, int ppid );
			
			process( int id, process& parent );
			
			~process();
			
			int id() const  { return its_id; }
			
			int getppid() const  { return its_ppid; }
			
			void orphan()  { its_ppid = 1; }
			
			void set_ppid( int ppid )  { its_ppid = ppid; }
			
			void clear_ppid()  { its_ppid = 0; }
			
			clock_t get_last_activity() const  { return its_last_activity; }
			
			const struct tms& get_times() const  { return its_times; }
			
			void add_user_cpu_time  ( clock_t delta )  { its_times.tms_utime += delta;  its_last_activity = clock(); }
			void add_system_cpu_time( clock_t delta )  { its_times.tms_stime += delta; }
			
			void accumulate_child_times( const struct tms& times );
			
			alarm_clock& get_alarm_clock()  { return its_alarm_clock; }
			
			const plus::string& get_cmdline() const;
			
			process_group& get_process_group() const;
			process_image& get_process_image() const;
			process_rsrcs& get_process_resources() const;
			
			void set_process_group( process_group& pg );
			void set_process_image( process_image& image );
			
			void reset_process_image();
			void reset_process_resources();
	};
	
}

#endif

