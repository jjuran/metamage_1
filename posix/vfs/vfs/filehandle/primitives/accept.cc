/*
	accept.cc
	---------
*/

#include "vfs/filehandle/primitives/accept.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/filehandle/methods/socket_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	filehandle_ptr accept( filehandle& that, sockaddr& client, socklen_t& len )
	{
		const vfs::socket_method_set& socket_methods = that.socket_methods();
		
		if ( socket_methods.accept == NULL )
		{
			p7::throw_errno( EINVAL );
		}
		
		return socket_methods.accept( &that, &client, &len );
	}
	
}

