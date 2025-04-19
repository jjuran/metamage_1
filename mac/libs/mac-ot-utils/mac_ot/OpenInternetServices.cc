/*
	OpenInternetServices.cc
	-----------------------
*/

#include "mac_ot/OpenInternetServices.hh"


#if CONFIG_OPEN_TRANSPORT_HEADERS

namespace mac {
namespace ot  {

InternetServices_opened OpenInternetServices_default_path()
{
	OTConfigurationRef cfg   = kDefaultInternetServicesPath;
	OTOpenFlags        flags = 0;
	
#if TARGET_API_MAC_CARBON
	
	InternetServices_opened opened;
	
	OSStatus& err = opened.error;
	
	opened.value = OTOpenInternetServicesInContext( cfg, flags, &err, NULL );
	
	return opened;
	
#else
	
	OSStatus err;
	
	InetSvcRef ref = OTOpenInternetServices( cfg, flags, &err );
	
	return (InternetServices_opened) (err ? err : (long) ref);
	
#endif
}

}
}

#else  // #if CONFIG_OPEN_TRANSPORT_HEADERS

int dummy;

#endif  // #if CONFIG_OPEN_TRANSPORT_HEADERS
