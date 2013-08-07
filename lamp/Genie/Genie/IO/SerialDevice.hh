/*	===============
 *	SerialDevice.hh
 *	===============
 */

#ifndef GENIE_IO_SERIALDEVICE_HH
#define GENIE_IO_SERIALDEVICE_HH

// Genie
#include "Genie/IO/Stream.hh"


namespace Genie
{
	
	IOPtr OpenSerialDevice( const plus::string&  portName,
	                        bool                 isPassive,
	                        bool                 nonblocking );
	
}

#endif

