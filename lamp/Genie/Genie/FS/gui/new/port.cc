/*
	gui/new/port.cc
	---------------
*/

#include "Genie/FS/gui/new/port.hh"

// POSIX
#include <sys/stat.h>

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/dir_method_set.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/FS/gui/port.hh"
#include "Genie/IO/Directory.hh"


namespace Genie
{
	
	static void remove_port_file( vfs::filehandle* that )
	{
		remove_port( that->GetFile().get() );
	}
	
	static IOPtr new_port_opendir( const FSTree* that )
	{
		FSTreePtr dir = new_port();
		
		return new DirHandle( dir.get(), &remove_port_file );
	}
	
	static const dir_method_set new_port_dir_methods =
	{
		NULL,
		NULL,
		NULL,
		&new_port_opendir
	};
	
	static const node_method_set new_port_methods =
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

