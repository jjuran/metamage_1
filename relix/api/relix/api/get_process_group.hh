/*
	get_process_group.hh
	--------------------
*/

#ifndef RELIX_API_GETPROCESSGROUP_HH
#define RELIX_API_GETPROCESSGROUP_HH


namespace relix
{
	
	class process_group;
	
	process_group* get_process_group( pid_t pgid );
	
}

#endif
