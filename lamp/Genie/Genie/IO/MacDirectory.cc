/*
	MacDirectory.cc
	---------------
*/

#include "Genie/IO/MacDirectory.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/general_method_set.hh"

// Genie
#include "Genie/FS/FSSpec.hh"


namespace Genie
{
	
	static vfs::node_ptr macdir_getfile( vfs::filehandle* that )
	{
		return static_cast< MacDirHandle& >( *that ).GetFile();
	}
	
	static const vfs::general_method_set macdir_general_methods =
	{
		NULL,
		NULL,
		&macdir_getfile,
	};
	
	static const vfs::filehandle_method_set macdir_methods =
	{
		NULL,
		NULL,
		&dir_stream_methods,
		&macdir_general_methods,
	};
	
	
	MacDirHandle::MacDirHandle( const Mac::FSDirSpec& dir )
	:
		DirHandle( macdir_methods ),
		its_dir_spec( dir )
	{
	}
	
	vfs::node_ptr MacDirHandle::GetFile()
	{
		return FSTreeFromFSDirSpec( its_dir_spec );
	}
	
}

