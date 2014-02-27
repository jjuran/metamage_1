/*
	ioctl.cc
	---------
*/

#include "vfs/filehandle/primitives/ioctl.hh"

// POSIX
#include <sys/ioctl.h>

// vfs
#include "vfs/filehandle.hh"
#include "vfs/filehandle/functions/nonblocking.hh"


namespace vfs
{
	
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
				that.IOCtl( request, argp );
				
				break;
		}
	}
	
}

