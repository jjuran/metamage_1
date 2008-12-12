/*	=========
 *	Device.hh
 *	=========
 */

#ifndef GENIE_IO_DEVICE_HH
#define GENIE_IO_DEVICE_HH

// POSIX
#include <fcntl.h>

// Genie
#include "Genie/IO/File.hh"


namespace Genie
{
	
	class DeviceHandle : public FileHandle
	{
		public:
			DeviceHandle( OpenFlags flags = O_RDWR ) : FileHandle( flags )
			{
			}
			
			virtual ~DeviceHandle()  {}
	};
	
}

#endif

