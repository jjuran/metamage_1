/*
	process_resources.cc
	--------------------
*/

#include "relix/task/process_resources.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/primitives/opendir.hh"

// relix
#include "relix/api/root.hh"
#include "relix/task/fd_map.hh"
#include "relix/task/fs_info.hh"


namespace relix
{
	
	process_resources::process_resources()
	:
		its_fs_info( relix::fs_info::create( opendir( *relix::root() ) ) ),
		its_fd_map ( relix::fd_map::create() )
	{
	}
	
	process_resources::process_resources( const process_resources& that )
	:
		its_fs_info( duplicate( *that.its_fs_info ) ),
		its_fd_map ( duplicate( *that.its_fd_map  ) )
	{
	}
	
	process_resources::~process_resources()
	{
	}
	
	fs_info& process_resources::get_fs_info() const
	{
		return *its_fs_info;
	}
	
	fd_map& process_resources::get_fd_map() const
	{
		return *its_fd_map;
	}
	
	void process_resources::unshare_fs_info()
	{
		its_fs_info = duplicate( *its_fs_info );
	}
	
	void process_resources::unshare_fd_map()
	{
		its_fd_map = duplicate( *its_fd_map );
	}
	
}

