/*
	cpu_time_checkpoint.cc
	----------------------
*/

#include "relix/time/cpu_time_checkpoint.hh"

// relix
#include "relix/api/current_process.hh"
#include "relix/task/process.hh"
#include "relix/task/schedule.hh"


namespace relix
{
	
	void system_cpu_time_checkpoint()
	{
		current_process().add_system_cpu_time( checkpoint_delta() );
	}
	
	void user_cpu_time_checkpoint()
	{
		current_process().add_user_cpu_time( checkpoint_delta() );
	}
	
}

