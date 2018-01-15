/*
	MacDirectory.cc
	---------------
*/

#include "Genie/IO/MacDirectory.hh"

// POSIX
#include <fcntl.h>

// vfs
#include "vfs/node.hh"
#include "vfs/node_ptr.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/general_method_set.hh"
#include "vfs/filehandle/types/directory.hh"

// Genie
#include "Genie/FS/FSSpec.hh"


namespace Genie
{
	
	struct Mac_dir_extra : vfs::dir_handle_extra
	{
		mac::types::VRefNum_DirID  dir_spec;
	};
	
	static vfs::node_ptr macdir_getfile( vfs::filehandle* that )
	{
		Mac_dir_extra& extra = *(Mac_dir_extra*) that->extra();
		
		return FSTreeFromFSDirSpec( extra.dir_spec );
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
		&vfs::dir_stream_methods,
		&macdir_general_methods,
	};
	
	
	vfs::filehandle_ptr Mac_dir_handle( const mac::types::VRefNum_DirID& dir )
	{
		using vfs::filehandle;
		
		filehandle* result = new filehandle( NULL,
		                                     O_RDONLY | O_DIRECTORY,
		                                     &macdir_methods,
		                                     sizeof (Mac_dir_extra),
		                                     &vfs::destroy_dir_handle );
		
		Mac_dir_extra& extra = *(Mac_dir_extra*) result->extra();
		
		extra.chained_destructor = NULL;
		extra.contents           = NULL;
		extra.dir_spec           = dir;
		
		return result;
	}
	
}
