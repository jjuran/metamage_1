/*
	conveying.cc
	------------
*/

#include "vfs/filehandle/primitives/conveying.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/socket_method_set.hh"


namespace vfs
{
	
	int conveying( filehandle& that )
	{
		if ( const filehandle_method_set* methods = that.methods() )
		{
			if ( const socket_method_set* socket_methods = methods->socket_methods )
			{
				if ( socket_methods->conveying )
				{
					return socket_methods->conveying( &that );
				}
			}
		}
		
		return 0;
	}
	
}

