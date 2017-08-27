/*
	Genie/FS/sys/app/exe.cc
	-----------------------
*/

#include "Genie/FS/sys/app/exe.hh"

// POSIX
#include <sys/stat.h>

// mac-file-utils
#include "mac_file/program_file.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/methods/link_method_set.hh"
#include "vfs/methods/node_method_set.hh"

// Genie
#include "Genie/FS/FSSpec.hh"


namespace Genie
{
	
	static vfs::node_ptr app_exe_resolve( const vfs::node* that )
	{
		return FSTreeFromFSSpec( mac::file::program_file() );
	}
	
	static const vfs::link_method_set app_exe_link_methods =
	{
		NULL,
		&app_exe_resolve
	};
	
	static const vfs::node_method_set app_exe_methods =
	{
		NULL,
		NULL,
		&app_exe_link_methods
	};
	
	vfs::node_ptr New_FSTree_sys_app_exe( const vfs::node*     parent,
	                                      const plus::string&  name,
	                                      const void*          args )
	{
		return new vfs::node( parent, name, S_IFLNK | 0777, &app_exe_methods );
	}
	
}
