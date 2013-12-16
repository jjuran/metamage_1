/*
	getpgid.cc
	----------
*/

#include "relix/syscall/getpgid.hh"

// relix-kernel
#include "relix/api/current_process.hh"
#include "relix/api/get_process.hh"
#include "relix/task/process.hh"
#include "relix/task/process_group.hh"


namespace relix
{
	
	int getpgid( int pid )
	{
		const process& proc = pid ? get_process( pid ) : current_process();
		
		return proc.get_process_group().id();
	}
	
}

