/*
	ProcessList.hh
	--------------
*/

#ifndef GENIE_PROCESSLIST_HH
#define GENIE_PROCESSLIST_HH

// POSIX
#include <sys/types.h>

// Genie
#include "Genie/Process.hh"


namespace Genie
{
	
	Process* lookup_process( pid_t pid );
	Process& get_process   ( pid_t pid );
	
	Process& NewProcess( Process& parent, pid_t ppid = 0 );
	
	Process& GetInitProcess();
	
	bool process_exists( pid_t pid );
	
	void* for_each_process( void* (*)( void*, pid_t, Process& ), void* = NULL );
	
	void spawn_process( const char* path, const char* const* argv, const char* const* envp );
	
	void spawn_process( const char* path, const char* const* argv );
	
	void spawn_process( const plus::string& program_args );
	
	void notify_reaper();
	
	void kill_all_processes();
	
}

#endif

