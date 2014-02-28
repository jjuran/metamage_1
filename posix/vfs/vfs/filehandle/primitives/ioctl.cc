/*
	ioctl.cc
	---------
*/

#include "vfs/filehandle/primitives/ioctl.hh"

// POSIX
#include <sys/ioctl.h>

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/filehandle/functions/nonblocking.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/general_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	void ioctl( filehandle& that, unsigned long request, int* argp )
	{
		switch ( request )
		{
			case FIONBIO:
				if ( *argp )
				{
					set_nonblocking( that );
				}
				else
				{
					clear_nonblocking( that );
				}
				
				break;
			
			default:
				if ( const filehandle_method_set* methods = that.methods() )
				{
					if ( const general_method_set* general_methods = methods->general_methods )
					{
						if ( general_methods->ioctl )
						{
							general_methods->ioctl( &that, request, argp );
							
							return;
						}
					}
					
				}
				
				p7::throw_errno( EINVAL );
				
				break;
		}
	}
	
}

