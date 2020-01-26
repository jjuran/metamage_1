/*
	new_tcp_socket.cc
	-----------------
*/

// mac-config
#include "mac_config/open-transport.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"

// relix-kernel
#include "relix/api/new_tcp_socket.hh"

// MacRelixLib
#include "new_OT_tcp_socket.hh"


namespace relix
{
	
	vfs::filehandle_ptr new_tcp_socket( bool nonblocking )
	{
		if ( CONFIG_OPEN_TRANSPORT )
		{
			return new_OT_tcp_socket( nonblocking );
		}
		
		poseven::throw_errno( EAFNOSUPPORT );
		
		return NULL;
	}
	
}
