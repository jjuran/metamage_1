/*
	spawn.cc
	--------
*/

#include "Genie/mnt/spawn.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/primitives/opendir.hh"

// relix-kernel
#include "relix/task/fs_info.hh"
#include "relix/task/process.hh"
#include "relix/task/process_resources.hh"
#include "relix/task/scheduler.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/Process.hh"
#include "Genie/ProcessList.hh"
#include "Genie/mnt/root.hh"


namespace Genie {

pid_t spawn_freemount_process()
{
	Process& parent = GetInitProcess();
	Process& child  = NewProcess( parent );
	
	relix::gCurrentProcess = &child;
	
	relix::process& proc = child.get_process();
	
	proc.unshare_per_fork();
	
	relix::fs_info& fs = proc.get_process_resources().get_fs_info();
	
	fs.chdir( opendir( freemount_root() ) );
	
	const pid_t tid = child.gettid();
	
	relix::mark_thread_inactive( tid );
	
	return tid;  // Same as pid
}

void terminate_freemount_process( pid_t pid )
{
	if ( Process* process = FindProcess( pid ) )
	{
		process->Terminate( 0 );
	}
}

}  // namespace Genie
