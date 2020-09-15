/*
	Genie/FS/sys/app/dir.cc
	-----------------------
*/

#include "Genie/FS/sys/app/dir.hh"

// POSIX
#include <sys/stat.h>

// mac-file-utils
#include "mac_file/parent_directory.hh"
#include "mac_file/program_file.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/methods/link_method_set.hh"
#include "vfs/methods/node_method_set.hh"

// Genie
#include "Genie/FS/FSSpec.hh"


namespace Genie
{
	
	using mac::file::parent_directory;
	using mac::file::program_file;
	
	
	static vfs::node_ptr app_dir_resolve( const vfs::node* that )
	{
		return FSTreeFromFSDirSpec( parent_directory( program_file() ) );
	}
	
	static const vfs::link_method_set app_dir_link_methods =
	{
		NULL,
		&app_dir_resolve
	};
	
	static const vfs::node_method_set app_dir_methods =
	{
		NULL,
		NULL,
		&app_dir_link_methods
	};
	
	vfs::node_ptr New_FSTree_sys_app_dir( const vfs::node*     parent,
	                                      const plus::string&  name,
	                                      const void*          args )
	{
		return new vfs::node( parent, name, S_IFLNK | 0777, &app_dir_methods );
	}
	
}
