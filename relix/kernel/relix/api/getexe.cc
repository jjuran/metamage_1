/*
	getexe.cc
	---------
*/

#include "relix/api/getexe.hh"

// vfs
#include "vfs/node.hh"

// relix-kernel
#include "relix/task/process.hh"
#include "relix/task/process_image.hh"


namespace relix
{
	
	vfs::node_ptr getexe( const process& proc )
	{
		return &proc.get_process_image().get_exe();
	}
	
}
