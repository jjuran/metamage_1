/*	===============
 *	SerialDevice.hh
 *	===============
 */

#ifndef GENIE_IO_SERIALDEVICE_HH
#define GENIE_IO_SERIALDEVICE_HH

// plus
#include "plus/string_fwd.hh"

// Genie
#include "Genie/IO/Stream.hh"


namespace Genie
{
	
	IOPtr OpenSerialDevice( const plus::string&  portName,
	                        bool                 isPassive,
	                        bool                 nonblocking );
	
}

#endif

