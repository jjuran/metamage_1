// OSErrno.cc

#include "OSErrno/OSErrno.hh"

// mac-sys-utils
#include "mac_sys/errno_from_mac_error.hh"


namespace OSErrno
{
	
	poseven::errno_t ErrnoFromOSStatus( ::OSStatus error )
	{
		int result = mac::sys::errno_from_mac_error( error );
		
		if ( result < 0 )
		{
			throw error;
		}
		
		return poseven::errno_t( result );
	}
	
}

