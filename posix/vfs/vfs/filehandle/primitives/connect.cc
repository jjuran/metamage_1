/*
	connect.cc
	----------
*/

#include "vfs/filehandle/primitives/connect.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/filehandle/methods/socket_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	void connect( filehandle& that, const sockaddr& server, socklen_t len )
	{
		const vfs::socket_method_set& socket_methods = that.socket_methods();
		
		if ( socket_methods.connect == NULL )
		{
			p7::throw_errno( EINVAL );
		}
		
		socket_methods.connect( &that, &server, len );
	}
	
}

