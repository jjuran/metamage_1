/*	==========
 *	Devices.hh
 *	==========
 */

#ifndef GENIE_DEVICES_HH
#define GENIE_DEVICES_HH

// vfs
#include "vfs/node_fwd.hh"

// Genie
#include "Genie/IO/IOPtr.hh"


namespace Genie
{
	
	IOPtr GetSimpleDeviceHandle( const vfs::node& file );
	
}

#endif

