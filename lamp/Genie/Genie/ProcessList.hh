/*
	ProcessList.hh
	--------------
*/

#ifndef GENIE_PROCESSLIST_HH
#define GENIE_PROCESSLIST_HH

// POSIX
#include <sys/types.h>

// Debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// Genie
#include "Genie/Process.hh"


namespace Genie
{
	
	const boost::intrusive_ptr< Process >& NewProcess( Process& parent );
	
	Process& GetInitProcess();
	
	bool process_exists( pid_t pid );
	
	void* for_each_process( void* (*)( void*, pid_t, Process& ), void* = NULL );
	
	void spawn_process( const char* program_path );
	
	void notify_reaper();
	
	void kill_all_processes();
	
}

#endif

