/*
	bind.cc
	-------
*/

#include "vfs/filehandle/primitives/bind.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/filehandle/methods/socket_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	void bind( filehandle& that, const sockaddr& local, socklen_t len )
	{
		const vfs::socket_method_set& socket_methods = that.socket_methods();
		
		if ( socket_methods.bind == NULL )
		{
			p7::throw_errno( EINVAL );
		}
		
		socket_methods.bind( &that, &local, len );
	}
	
}

