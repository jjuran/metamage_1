/*
	Genie/FS/sys/app/exe.cc
	-----------------------
*/

#include "Genie/FS/sys/app/exe.hh"

// POSIX
#include <sys/stat.h>

// mac-sys-utils
#include "mac_sys/get_process_file.hh"

// Genie
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/link_method_set.hh"
#include "Genie/FS/node_method_set.hh"


namespace Genie
{
	
	static FSTreePtr app_exe_resolve( const FSTree* that )
	{
		return FSTreeFromFSSpec( mac::sys::get_process_file() );
	}
	
	static const link_method_set app_exe_link_methods =
	{
		NULL,
		&app_exe_resolve
	};
	
	static const node_method_set app_exe_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&app_exe_link_methods
	};
	
	FSTreePtr New_FSTree_sys_app_exe( const FSTree*        parent,
	                                  const plus::string&  name,
	                                  const void*          args )
	{
		return new FSTree( parent, name, S_IFLNK | 0777, &app_exe_methods );
	}
	
}

