/*
	ProcessList.hh
	--------------
*/

#ifndef GENIE_PROCESSLIST_HH
#define GENIE_PROCESSLIST_HH

// POSIX
#include <sys/types.h>


namespace Genie
{
	
	class Process;
	
	
	Process* lookup_process( pid_t pid );
	Process& get_process   ( pid_t pid );
	
	Process& NewProcess( Process& parent );
	
	Process& NewThread( Process& caller );
	
	Process& GetInitProcess();
	
	bool process_exists( pid_t pid );
	
	void* for_each_process( void* (*)( void*, pid_t, Process& ), void* = 0 );  // NULL
	
	void spawn_process( const char* path, const char* const* argv, const char* const* envp );
	
	void spawn_process( const char* path, const char* const* argv );
	
	void spawn_process( const char* path );
	
	void destroy_pending();
	
	void notify_reaper( Process* released );
	
	bool is_ready_to_exit();
	
}

#endif
