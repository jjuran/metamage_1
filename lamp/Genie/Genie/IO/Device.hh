/*	=========
 *	Device.hh
 *	=========
 */

#ifndef GENIE_IO_DEVICE_HH
#define GENIE_IO_DEVICE_HH

// Genie
#include "Genie/IO/File.hh"


namespace Genie
{
	
	class DeviceHandle : public FileHandle
	{
		public:
			virtual ~DeviceHandle()  {}
	};
	
}

#endif

