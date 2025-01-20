/*
	OpenTransport_share.hh
	----------------------
*/

#include "mac_app/OpenTransport_share.hh"

#if defined( __APPLE__ )  ||  TARGET_API_MAC_CARBON
#define OTCARBONAPPLICATION 1
#endif

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// mac-config
#include "mac_config/open-transport.hh"

#if CONFIG_OPEN_TRANSPORT_HEADERS

// Mac OS
#ifndef __OPENTRANSPORT__
#include <OpenTransport.h>
#endif

// Standard C
#include <stdlib.h>


namespace mac {
namespace app {
	
	static int OpenTransport_refcount = 0;
	
	
	void default_OSStatus_handler( OSStatus err )
	{
		abort();
	}
	
	void InitOpenTransport_shared( OSStatus_handler handler )
	{
		if ( OpenTransport_refcount == 0 )
		{
			OSStatus err = ::InitOpenTransport();
			
			if ( err != noErr )
			{
				handler( err );
			}
		}
		
		++OpenTransport_refcount;
	}
	
	void CloseOpenTransport_shared()
	{
		if ( --OpenTransport_refcount == 0 )
		{
			::CloseOpenTransport();
		}
	}
	
}
}

#else  // #if CONFIG_OPEN_TRANSPORT_HEADERS

void dummy()
{
	// Silence a warning about a unit with no symbols
}

#endif  // #if CONFIG_OPEN_TRANSPORT_HEADERS
