/*
	process_resources.cc
	--------------------
*/

#include "relix/task/process_resources.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/functions/root.hh"
#include "vfs/primitives/opendir.hh"

// relix
#include "relix/task/fs_info.hh"


namespace relix
{
	
	process_resources::process_resources()
	:
		its_fs_info( relix::fs_info::create( opendir( *vfs::root() ) ) )
	{
	}
	
	process_resources::process_resources( const process_resources& that )
	:
		its_fs_info( duplicate( *that.its_fs_info ) )
	{
	}
	
	process_resources::~process_resources()
	{
	}
	
	fs_info& process_resources::get_fs_info() const
	{
		return *its_fs_info;
	}
	
	void process_resources::unshare_fs_info()
	{
		its_fs_info = duplicate( *its_fs_info );
	}
	
}

