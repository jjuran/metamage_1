/*
	MacDirectory.hh
	---------------
*/

#ifndef GENIE_IO_MACDIRECTORY_HH
#define GENIE_IO_MACDIRECTORY_HH

// mac-types
#include "mac_types/VRefNum_DirID.hh"

// vfs
#include "vfs/filehandle_ptr.hh"


namespace Genie
{
	
	vfs::filehandle_ptr Mac_dir_handle( const mac::types::VRefNum_DirID& dir );
	
}

#endif
