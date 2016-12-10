/*
	spawn.hh
	--------
*/

#ifndef GENIE_MNT_SPAWN_HH
#define GENIE_MNT_SPAWN_HH

// POSIX
#include <sys/types.h>


namespace Genie
{
	
	pid_t spawn_freemount_process();
	
	void terminate_freemount_process( pid_t pid );
	
}

#endif
