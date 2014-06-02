/*
	process.hh
	----------
*/

#ifndef RELIX_TASK_PROCESS_HH
#define RELIX_TASK_PROCESS_HH

// POSIX
#include <signal.h>
#include <time.h>
#include <sys/times.h>

// Debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// plus
#include "plus/ref_count.hh"
#include "plus/string.hh"

// relix-kernel
#include "relix/signal/sigset_t.hh"
#include "relix/task/alarm_clock.hh"


struct sigaction;


namespace relix
{
	
	class process_group;
	class process_image;
	class process_resources;
	class signal_handlers;
	
	typedef process_resources process_rsrcs;
	
	
	class process : public plus::ref_count< process >
	{
		private:
			int its_id;
			int its_ppid;
			int its_status;
			
			bool it_is_stopped;
			
			plus::string its_name;
			
			clock_t its_last_activity;
			
			struct tms its_times;
			
			alarm_clock its_alarm_clock;
			
			sigset_t its_pending_signals;
			
			boost::intrusive_ptr< process_group > its_process_group;
			boost::intrusive_ptr< process_image > its_process_image;
			boost::intrusive_ptr< process_rsrcs > its_process_resources;
			
			boost::intrusive_ptr< relix::signal_handlers > its_signal_handlers;
		
		public:
			explicit process();
			
			process( int id, process& parent );
			
			~process();
			
			int id() const  { return its_id; }
			
			int getppid() const  { return its_ppid; }
			
			void orphan()  { its_ppid = 1; }
			
			void set_ppid( int ppid )  { its_ppid = ppid; }
			
			void clear_ppid()  { its_ppid = 0; }
			
			int status() const  { return its_status; }
			
			void set_status( int status )  { its_status = status; }
			
			const plus::string& name() const  { return its_name; }
			
			void set_name( const plus::string& name )  { its_name = name; }
			
			clock_t get_last_activity() const  { return its_last_activity; }
			
			const struct tms& get_times() const  { return its_times; }
			
			void add_user_cpu_time  ( clock_t delta )  { its_times.tms_utime += delta;  its_last_activity = clock(); }
			void add_system_cpu_time( clock_t delta )  { its_times.tms_stime += delta; }
			
			void accumulate_child_times( const struct tms& times );
			
			alarm_clock& get_alarm_clock()  { return its_alarm_clock; }
			
			const plus::string& get_cmdline() const;
			
			sigset_t signals_pending() const  { return its_pending_signals; }
			
			void set_pending_signal( int sig )
			{
				sigaddset( &its_pending_signals, sig );
			}
			
			void clear_pending_signal( int sig )
			{
				sigdelset( &its_pending_signals, sig );
			}
			
			process_group& get_process_group() const;
			process_image& get_process_image() const;
			process_rsrcs& get_process_resources() const;
			
			void set_process_group( process_group& pg );
			void set_process_image( process_image& image );
			
			bool is_zombie() const  { return ! its_process_resources.get(); }
			
			void zombify();
			
			const struct sigaction& get_sigaction( int signo ) const;
			
			void set_sigaction( int signo, const struct sigaction& action );
			
			void unshare_signal_handlers();
			
			void reset_signal_handlers();
			
			void unshare_per_fork();
			
			void back_up_memory();
			void restore_memory();
			
			void switch_in();
			void switch_out();
			
			bool is_stopped() const  { return it_is_stopped; }
			
			void signal_stop();
			void signal_continue();
	};
	
}

#endif
