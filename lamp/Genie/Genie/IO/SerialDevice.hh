/*	===============
 *	SerialDevice.hh
 *	===============
 */

#ifndef GENIE_IO_SERIALDEVICE_HH
#define GENIE_IO_SERIALDEVICE_HH

// plus
#include "plus/string_fwd.hh"

// vfs
#include "vfs/filehandle_ptr.hh"

// Genie
#include "Genie/IO/Stream.hh"


namespace Genie
{
	
	vfs::filehandle_ptr OpenSerialDevice( const plus::string&  portName,
	                                      bool                 isPassive,
	                                      bool                 nonblocking );
	
}

#endif

