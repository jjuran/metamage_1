/*
	boot_volume.hh
	--------------
*/

#ifndef MACFILE_BOOTVOLUME_HH
#define MACFILE_BOOTVOLUME_HH

// mac-file-utils
#include "mac_file/system_file.hh"


namespace mac  {
namespace file {
	
	inline
	short boot_volume()
	{
		return system_file().vRefNum;
	}
	
}
}

#endif
