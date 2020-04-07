/*
	new_OT_tcp_socket.hh
	--------------------
*/

#ifndef NEWOTTCPSOCKET_HH
#define NEWOTTCPSOCKET_HH

// vfs
#include "vfs/filehandle_ptr.hh"


namespace relix
{
	
	vfs::filehandle_ptr new_OT_tcp_socket( bool nonblocking );
	
}

#endif
