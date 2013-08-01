/*	=======
 *	Pipe.hh
 *	=======
 */

#ifndef GENIE_IO_PIPE_HH
#define GENIE_IO_PIPE_HH

// vfs
#include "vfs/filehandle_ptr.hh"


namespace Genie
{
	
	struct pipe_ends
	{
		vfs::filehandle_ptr reader;
		vfs::filehandle_ptr writer;
	};
	
	pipe_ends new_pipe( int nonblock );
	
}

#endif

