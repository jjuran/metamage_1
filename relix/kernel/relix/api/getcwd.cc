/*
	getcwd.cc
	---------
*/

#include "relix/api/getcwd.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/filehandle/primitives/get_file.hh"

// relix-kernel
#include "relix/api/current_process.hh"
#include "relix/task/fs_info.hh"
#include "relix/task/process.hh"
#include "relix/task/process_resources.hh"


namespace relix
{
	
	vfs::node_ptr getcwd( const process& proc )
	{
		return get_file( *proc.get_process_resources().get_fs_info().getcwd() );
	}
	
	vfs::node_ptr getcwd()
	{
		return getcwd( current_process() );
	}
	
}

