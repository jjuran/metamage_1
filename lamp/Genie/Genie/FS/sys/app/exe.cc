/*
	Genie/FS/sys/app/exe.cc
	-----------------------
*/

#include "Genie/FS/sys/app/exe.hh"

// POSIX
#include <sys/stat.h>

// Genie
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/link_method_set.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/Utilities/GetAppFile.hh"


namespace Genie
{
	
	static FSTreePtr app_exe_resolve( const FSTree* node )
	{
		return FSTreeFromFSSpec( GetAppFile(), false );
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
		NULL,
		NULL,
		NULL,
		NULL,
		&app_exe_link_methods
	};
	
	FSTreePtr New_FSTree_sys_app_exe( const FSTreePtr&     parent,
	                                  const plus::string&  name,
	                                  const void*          args )
	{
		return new FSTree( parent, name, S_IFLNK | 0777, &app_exe_methods );
	}
	
}

