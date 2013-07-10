/*
	shutdown.cc
	----------
*/

#include "vfs/filehandle/primitives/shutdown.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/filehandle/methods/socket_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	void shutdown( filehandle& that, int how )
	{
		const vfs::socket_method_set& socket_methods = that.socket_methods();
		
		if ( socket_methods.shutdown == NULL )
		{
			p7::throw_errno( EINVAL );
		}
		
		socket_methods.shutdown( &that, how );
	}
	
}

