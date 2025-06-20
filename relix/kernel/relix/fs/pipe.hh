/*
	pipe.hh
	-------
*/

#ifndef RELIX_FS_PIPE_HH
#define RELIX_FS_PIPE_HH

// iota
#include "iota/class.hh"

// vfs
#include "vfs/filehandle_ptr.hh"


namespace relix
{
	
	struct pipe_ends
	{
		vfs::filehandle_ptr reader;
		vfs::filehandle_ptr writer;
		
		NO_NEW_DELETE
	};
	
	pipe_ends new_pipe( int nonblock );
	
}

#endif
