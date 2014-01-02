/*
	cpu_time_checkpoint.hh
	----------------------
*/

#ifndef RELIX_TIME_CPUTIMECHECKPOINT_HH
#define RELIX_TIME_CPUTIMECHECKPOINT_HH


namespace relix
{
	
	void system_cpu_time_checkpoint();
	
	void user_cpu_time_checkpoint();
	
	inline void enter_system()
	{
		user_cpu_time_checkpoint();
	}
	
	inline void leave_system()
	{
		system_cpu_time_checkpoint();
	}
	
}

#endif

