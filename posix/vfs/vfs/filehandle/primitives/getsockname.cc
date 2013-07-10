/*
	getsockname.cc
	--------------
*/

#include "vfs/filehandle/primitives/getsockname.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/filehandle/methods/socket_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	const sockaddr& getsockname( filehandle& that )
	{
		const vfs::socket_method_set& socket_methods = that.socket_methods();
		
		if ( socket_methods.getsockname == NULL )
		{
			p7::throw_errno( EINVAL );
		}
		
		const sockaddr* result = socket_methods.getsockname( &that );
		
		if ( result == NULL )
		{
			p7::throw_errno( EINVAL );
		}
		
		return *result;
	}
	
}

