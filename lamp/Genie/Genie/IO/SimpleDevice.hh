/*	===============
 *	SimpleDevice.hh
 *	===============
 */

#ifndef GENIE_IO_SIMPLEDEVICE_HH
#define GENIE_IO_SIMPLEDEVICE_HH

// vfs
#include "vfs/filehandle_ptr.hh"
#include "vfs/node_fwd.hh"


namespace Genie
{
	
	vfs::filehandle_ptr GetSimpleDeviceHandle( const vfs::node& file );
	
}

#endif

