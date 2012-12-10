/*
	mmap.hh
	-------
*/

#ifndef VFS_FILEHANDLE_PRIMITIVES_MMAP_HH
#define VFS_FILEHANDLE_PRIMITIVES_MMAP_HH

// POSIX
#include <sys/types.h>

// vfs
#include "vfs/filehandle_fwd.hh"
#include "vfs/memory_mapping_ptr.hh"


namespace vfs
{
	
	memory_mapping_ptr mmap( filehandle* that, size_t length, int prot, int flags, off_t offset );
	
}

#endif

