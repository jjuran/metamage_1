// Nostalgia/OpenTransportProviders.hh

#ifndef NOSTALGIA_OPENTRANSPORTPROVIDERS_HH
#define NOSTALGIA_OPENTRANSPORTPROVIDERS_HH

#ifndef __OPENTRANSPORTPROVIDERS__
// OpenTransportProviders.h uses FSSpec but doesn't include Files.h
#ifndef __FILES__
#include <Files.h>
#endif
#include <OpenTransportProviders.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if TARGET_API_MAC_CARBON
	
	inline pascal InetSvcRef OTOpenInternetServices( OTConfigurationRef   config,
	                                                 OTOpenFlags          flags,
	                                                 OSStatus            *err )
	{
		return OTOpenInternetServicesInContext( config, flags, err, NULL );
	}
	
#endif

#ifdef __cplusplus
}
#endif

#endif

