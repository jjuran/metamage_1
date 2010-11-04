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
	
	const boost::intrusive_ptr< Process >& NewProcess( Process& parent, pid_t ppid = 0 );
	
	Process& GetInitProcess();
	
	bool process_exists( pid_t pid );
	
	void* for_each_process( void* (*)( void*, pid_t, Process& ), void* = NULL );
	
	void spawn_process( const plus::string& program_args );
	
	void notify_reaper();
	
	void kill_all_processes();
	
}

#endif

