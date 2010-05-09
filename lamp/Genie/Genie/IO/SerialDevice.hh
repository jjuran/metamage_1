/*	===============
 *	SerialDevice.hh
 *	===============
 */

#ifndef GENIE_IO_SERIALDEVICE_HH
#define GENIE_IO_SERIALDEVICE_HH

// Genie
#include "Genie/IO/Device.hh"


namespace Genie
{
	
	boost::shared_ptr< IOHandle > OpenSerialDevice( const plus::string&  portName,
	                                                bool                 isPassive,
	                                                bool                 nonblocking );
	
}

#endif

