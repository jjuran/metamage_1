/*
	buffer_file.hh
	--------------
*/

#ifndef VFS_FILEHANDLE_TYPES_BUFFERFILE_HH
#define VFS_FILEHANDLE_TYPES_BUFFERFILE_HH

// vfs
#include "vfs/filehandle_ptr.hh"
#include "vfs/node_fwd.hh"


namespace vfs
{
	
	filehandle_ptr open_buffer_file( const node&  file,
	                                 int          flags,
	                                 char*        addr,
	                                 std::size_t  size );
	
}

#endif

