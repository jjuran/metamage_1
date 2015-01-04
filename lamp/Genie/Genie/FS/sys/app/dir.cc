/*
	Genie/FS/sys/app/dir.cc
	-----------------------
*/

#include "Genie/FS/sys/app/dir.hh"

// POSIX
#include <sys/stat.h>

// vfs
#include "vfs/methods/link_method_set.hh"
#include "vfs/methods/node_method_set.hh"

// Genie
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/FSTree.hh"
#include "Genie/Utilities/GetAppFolder.hh"


namespace Genie
{
	
	static FSTreePtr app_dir_resolve( const FSTree* that )
	{
		return FSTreeFromFSDirSpec( GetAppFolder() );
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
	
	FSTreePtr New_FSTree_sys_app_dir( const FSTree*        parent,
	                                  const plus::string&  name,
	                                  const void*          args )
	{
		return new FSTree( parent, name, S_IFLNK | 0777, &app_dir_methods );
	}
	
}
