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
#include "Genie/IO/VirtualDirectory.hh"


namespace Genie
{
	
	class OpenWindowHandle : public VirtualDirHandle
	{
		public:
			OpenWindowHandle( const FSTreePtr& tree );
			
			~OpenWindowHandle();
	};
	
	OpenWindowHandle::OpenWindowHandle( const FSTreePtr& tree ) : VirtualDirHandle( tree )
	{
	}
	
	OpenWindowHandle::~OpenWindowHandle()
	{
		remove_port( GetFile().get() );
	}
	
	static IOPtr new_port_chdir( const FSTree* node )
	{
		FSTreePtr dir = new_port();
		
		return new OpenWindowHandle( dir );
	}
	
	static const dir_method_set new_port_dir_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		&new_port_chdir
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
		NULL,
		NULL,
		&new_port_dir_methods
	};
	
	
	FSTreePtr New_new_port( const FSTreePtr&     parent,
	                        const plus::string&  name,
	                        const void*          args )
	{
		return new FSTree( parent, name, S_IFDIR | 0100, &new_port_methods );
	}
	
}

