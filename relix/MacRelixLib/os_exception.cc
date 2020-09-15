/*
	os_exception.cc
	---------------
*/

#include "relix/api/os_exception.hh"

// mac-sys-utils
#include "mac_sys/errno_from_mac_error.hh"

// Nitrogen
#include "Mac/Toolbox/Types/OSStatus.hh"


namespace relix
{
	
	int errno_from_os_exception()
	{
		try
		{
			throw;
		}
		catch ( const Mac::OSStatus& err )
		{
			return mac::sys::errno_from_mac_error( err );
		}
		
		// Not reached, but try telling that to Metrowerks C++
		return 0;
	}
	
}
