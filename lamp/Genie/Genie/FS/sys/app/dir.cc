/*
	Genie/FS/sys/app/dir.cc
	-----------------------
*/

#include "Genie/FS/sys/app/dir.hh"

// POSIX
#include <sys/stat.h>

// Genie
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/link_method_set.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/Utilities/GetAppFolder.hh"


namespace Genie
{
	
	static FSTreePtr app_dir_resolve( const FSTree* node )
	{
		return FSTreeFromFSDirSpec( GetAppFolder(), false );
	}
	
	static const link_method_set app_dir_link_methods =
	{
		NULL,
		&app_dir_resolve
	};
	
	static const node_method_set app_dir_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&app_dir_link_methods
	};
	
	FSTreePtr New_FSTree_sys_app_dir( const FSTreePtr&     parent,
	                                  const plus::string&  name,
	                                  const void*          args )
	{
		return new FSTree( parent, name, S_IFLNK | 0777, &app_dir_methods );
	}
	
}

