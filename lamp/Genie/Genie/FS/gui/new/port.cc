/*
	gui/new/port.cc
	---------------
*/

#include "Genie/FS/gui/new/port.hh"

// POSIX
#include <sys/stat.h>

// vfs
#include "vfs/filehandle/primitives/get_file.hh"
#include "vfs/filehandle/types/directory.hh"
#include "vfs/methods/dir_method_set.hh"
#include "vfs/methods/node_method_set.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/gui/port.hh"


namespace Genie
{
	
	static void remove_port_file( vfs::filehandle* that )
	{
		remove_port( get_file( *that ).get() );
	}
	
	static vfs::filehandle_ptr new_port_opendir( const FSTree* that )
	{
		FSTreePtr dir = new_port();
		
		return new vfs::dir_handle( dir.get(), &remove_port_file );
	}
	
	static const vfs::dir_method_set new_port_dir_methods =
	{
		NULL,
		NULL,
		NULL,
		&new_port_opendir
	};
	
	static const vfs::node_method_set new_port_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&new_port_dir_methods
	};
	
	
	FSTreePtr New_new_port( const FSTree*        parent,
	                        const plus::string&  name,
	                        const void*          args )
	{
		return new FSTree( parent, name, S_IFDIR | 0100, &new_port_methods );
	}
	
}
