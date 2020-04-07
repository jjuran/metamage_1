/*
	con_group.hh
	------------
*/

#ifndef RELIX_FS_CONGROUP_HH
#define RELIX_FS_CONGROUP_HH

// vfs
#include "vfs/filehandle/types/dynamic_group.hh"


namespace relix
{
	
	vfs::dynamic_group& get_con_group();
	
}

#endif
