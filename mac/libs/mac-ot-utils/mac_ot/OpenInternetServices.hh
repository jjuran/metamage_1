/*
	OpenInternetServices.hh
	-----------------------
*/

#ifndef MACOT_OPENINTERNETSERVICES_HH
#define MACOT_OPENINTERNETSERVICES_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// mac-config
#include "mac_config/open-transport.hh"

#if CONFIG_OPEN_TRANSPORT_HEADERS

// Mac OS
// OpenTransportProviders.h depends on FSSpec but doesn't include Files.h.
#ifndef __FILES__
#include <Files.h>
#endif
#ifndef __OPENTRANSPORTPROVIDERS__
#include <OpenTransportProviders.h>
#endif


namespace mac {
namespace ot  {
	
	/*
		InternetServices_opened is the result type for our custom
		OpenInternetServices_default_path() function.  In Carbon,
		it's a struct that holds both an OSStatus error code and
		an InetSvcRef value.  Pre-Carbon, we can rely on the facts
		that OSStatus error codes are always negative (high bit set)
		and memory addresses are always under 2 GiB (high bit clear),
		and therefore a 32-bit value can code for either, using the
		high bit as a discriminator.
	*/
	
	struct InternetServices_opened_struct
	{
		OSStatus   error;
		InetSvcRef value;
	};
	
	inline
	OSStatus error( InternetServices_opened_struct result )
	{
		return result.error;
	}
	
	inline
	InetSvcRef value( InternetServices_opened_struct result )
	{
		return result.value;
	}
	
	inline
	bool is_error( InternetServices_opened_struct result )
	{
		return error( result );
	}
	
	enum InternetServices_opened_enum
	{
		kInternetServices_opened_max = 32767 << 16,
		kInternetServices_opened_min = -kInternetServices_opened_max,
	};
	
	inline
	bool is_error( InternetServices_opened_enum result )
	{
		return result < 0;
	}
	
	inline
	OSStatus error( InternetServices_opened_enum result )
	{
		return is_error( result ) ? result : 0;
	}
	
	inline
	InetSvcRef value( InternetServices_opened_enum result )
	{
		return is_error( result ) ? NULL : (InetSvcRef) result;
	}
	
#if TARGET_API_MAC_CARBON
	
	typedef InternetServices_opened_struct InternetServices_opened;
	
#else
	
	typedef InternetServices_opened_enum InternetServices_opened;
	
#endif
	
	InternetServices_opened OpenInternetServices_default_path();
	
}
}

#endif  // #if CONFIG_OPEN_TRANSPORT_HEADERS

#endif
