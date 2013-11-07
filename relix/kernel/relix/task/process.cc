/*
	process.cc
	----------
*/

#include "relix/task/process.hh"

// relix
#include "relix/task/process_group.hh"


namespace relix
{
	
	process::process( int id, int ppid, process_group& pg )
	:
		its_id           ( id   ),
		its_ppid         ( ppid ),
		its_last_activity(      ),
		its_process_group( &pg  )
	{
		// Reset resource utilization on fork
		
		its_times.tms_utime  = 0;
		its_times.tms_stime  = 0;
		its_times.tms_cutime = 0;
		its_times.tms_cstime = 0;
	}
	
	process::~process()
	{
	}
	
	void process::accumulate_child_times( const struct tms& times )
	{
		its_times.tms_cutime += times.tms_utime + times.tms_cutime;
		its_times.tms_cstime += times.tms_stime + times.tms_cstime;
	}
	
	process_group& process::get_process_group() const
	{
		return *its_process_group;
	}
	
	void process::set_process_group( process_group& pg )
	{
		its_process_group = &pg;
	}
	
}

