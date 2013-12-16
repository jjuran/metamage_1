/*
	stream.cc
	---------
*/

#include "vfs/stream.hh"

// vfs
#include "vfs/filehandle.hh"


namespace vfs
{
	
	void stream::send_fd( filehandle& fh )
	{
		its_fds.push_back( &fh );
	}
	
	filehandle_ptr stream::recv_fd( bool nonblocking, try_again_f try_again )
	{
		while ( its_fds.empty() )
		{
			try_again( nonblocking );
		}
		
		filehandle_ptr result = its_fds.front();
		
		its_fds.pop_front();
		
		return result;
	}
	
}

