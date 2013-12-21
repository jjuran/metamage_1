/*
	new_tcp_socket.hh
	-----------------
*/

#ifndef RELIX_API_NEWTCPSOCKET_HH
#define RELIX_API_NEWTCPSOCKET_HH

// vfs
#include "vfs/filehandle_ptr.hh"


namespace relix
{
	
	vfs::filehandle_ptr new_tcp_socket( bool nonblocking );
	
}

#endif

