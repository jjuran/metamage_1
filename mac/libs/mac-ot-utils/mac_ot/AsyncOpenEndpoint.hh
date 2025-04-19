/*
	AsyncOpenEndpoint.hh
	--------------------
*/

#ifndef MACOT_ASYNCOPENENDPOINT_HH
#define MACOT_ASYNCOPENENDPOINT_HH

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


#if TARGET_API_MAC_CARBON

static inline
pascal OSStatus OTAsyncOpenEndpoint( OTConfigurationRef  config,
                                     OTOpenFlags         flags,
                                     TEndpointInfo*      info,
                                     OTNotifyUPP         notifier,
                                     void*               extra )
{
	return OTAsyncOpenEndpointInContext( config,
	                                     flags,
	                                     info,
	                                     notifier,
	                                     extra,
	                                     NULL );
}

#endif

namespace mac {
namespace ot  {
	
	inline
	OSStatus AsyncOpenEndpoint( OTConfigurationRef  config,
	                            OTOpenFlags         flags,
	                            TEndpointInfo*      info,
	                            OTNotifyUPP         notifier,
	                            void*               extra )
	{
		return OTAsyncOpenEndpoint( config, flags, info, notifier, extra );
	}
	
	inline
	OSStatus AsyncOpenEndpoint( OTConfigurationRef  config,
	                            OTNotifyUPP         notifier,
	                            void*               extra )
	{
		return AsyncOpenEndpoint( config, 0, NULL, notifier, extra );
	}
	
	inline
	OSStatus AsyncOpenEndpoint( const char*  config_str,
	                            OTNotifyUPP  notifier,
	                            void*        extra )
	{
		OSStatus err = kOTBadConfigurationErr;
		
		if ( OTConfigurationRef config = OTCreateConfiguration( config_str ) )
		{
			err = AsyncOpenEndpoint( config, notifier, extra );
			
			if ( err )
			{
				OTDestroyConfiguration( config );
			}
		}
		
		return err;
	}
	
}
}

#endif  // #if CONFIG_OPEN_TRANSPORT_HEADERS

#endif
