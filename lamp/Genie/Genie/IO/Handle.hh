/*
	Handle.hh
	---------
*/

#ifndef GENIE_IO_HANDLE_HH
#define GENIE_IO_HANDLE_HH

// nucleus
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif

// Nitrogen
#ifndef MAC_MEMORY_TYPES_HANDLE_HH
#include "Mac/Memory/Types/Handle.hh"
#endif

// vfs
#include "vfs/filehandle.hh"


namespace vfs
{
	
	struct filehandle_method_set;
	
}

namespace Genie
{
	
	struct Mac_Handle_extra
	{
		char** handle;
	};
	
	extern const vfs::filehandle_method_set Mac_Handle_methods;
	
	vfs::filehandle* new_Handle_handle( const vfs::node&               file,
	                                    int                            flags,
	                                    nucleus::owned< Mac::Handle >  h );
	
}

#endif
