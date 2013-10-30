/*
	alarm.cc
	--------
*/

#include "relix/syscall/alarm.hh"

// relix-kernel
#include "relix/api/current_process.hh"
#include "relix/task/process.hh"


namespace relix
{
	
	unsigned int alarm( unsigned int seconds )
	{
		return current_process().get_alarm_clock().set( seconds );
	}
	
}

