/*
	listen.cc
	---------
*/

#include "vfs/filehandle/primitives/listen.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/filehandle/methods/socket_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	void listen( filehandle& that, int backlog )
	{
		const vfs::socket_method_set& socket_methods = that.socket_methods();
		
		if ( socket_methods.listen == NULL )
		{
			p7::throw_errno( EINVAL );
		}
		
		socket_methods.listen( &that, backlog );
	}
	
}

