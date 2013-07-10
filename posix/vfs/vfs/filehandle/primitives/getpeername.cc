/*
	getpeername.cc
	--------------
*/

#include "vfs/filehandle/primitives/getpeername.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/filehandle/methods/socket_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	const sockaddr& getpeername( filehandle& that )
	{
		const vfs::socket_method_set& socket_methods = that.socket_methods();
		
		if ( socket_methods.getpeername == NULL )
		{
			p7::throw_errno( EINVAL );
		}
		
		const sockaddr* result = socket_methods.getpeername( &that );
		
		if ( result == NULL )
		{
			p7::throw_errno( EINVAL );
		}
		
		return *result;
	}
	
}

