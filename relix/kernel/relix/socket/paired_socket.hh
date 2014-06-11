/*
	paired_socket.hh
	----------------
*/

#ifndef RELIX_SOCKET_PAIREDSOCKET_HH
#define RELIX_SOCKET_PAIREDSOCKET_HH

// vfs
#include "vfs/stream.hh"


namespace relix
{
	
	vfs::filehandle_ptr new_paired_socket( vfs::stream&  input,
	                                       vfs::stream&  output,
	                                       bool          nonblocking );
	
}

#endif
