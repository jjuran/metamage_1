/*
	unix_domain_socket.hh
	---------------------
*/

#ifndef RELIX_SOCKET_UNIXDOMAINSOCKET_HH
#define RELIX_SOCKET_UNIXDOMAINSOCKET_HH

// vfs
#include "vfs/filehandle_ptr.hh"


namespace relix
{
	
	vfs::filehandle_ptr new_unix_domain_socket( int flags );
	
}

#endif
