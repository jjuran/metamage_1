/*
	ioctl.cc
	---------
*/

#include "vfs/filehandle/primitives/ioctl.hh"

// vfs
#include "vfs/filehandle.hh"


namespace vfs
{
	
	void ioctl( filehandle* that, unsigned long request, int* argp )
	{
		that->IOCtl( request, argp );
	}
	
}

