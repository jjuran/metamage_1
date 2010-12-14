/*	================
 *	OpenTransport.hh
 *	================
 */

#ifndef CLASSICTOOLBOX_OPENTRANSPORT_HH
#define CLASSICTOOLBOX_OPENTRANSPORT_HH

// Nostalgia
#include "Nostalgia/OpenTransport.hh"

// Nitrogen
#ifndef NITROGEN_OPENTRANSPORT_HH
#include "Nitrogen/OpenTransport.hh"
#endif


namespace Nitrogen
{
	
	inline void InitOpenTransport()
	{
		Mac::ThrowOSStatus( ::InitOpenTransport() );
	}
	
	using ::CloseOpenTransport;
	
	inline nucleus::owned< EndpointRef >
	//
	OTOpenEndpoint( nucleus::owned< OTConfigurationRef >  config,
	                TEndpointInfo*                        info = NULL )
	{
		::OSStatus err;
		
		EndpointRef result = ::OTOpenEndpoint( config.release(),
		                                       OTOpenFlags( 0 ),
		                                       info,
		                                       &err );
		
		Mac::ThrowOSStatus( err );
		
		return nucleus::owned< EndpointRef >::seize( result );
	}
	
	inline void
	//
	OTAsyncOpenEndpoint( nucleus::owned< OTConfigurationRef >  config,
	                     TEndpointInfo*                        info,
	                     OTNotifyUPP                           notifier,
	                     void*                                 context )
	{
		OSStatus err = ::OTAsyncOpenEndpoint( config.release(),
		                                      OTOpenFlags( 0 ),
		                                      info,
		                                      notifier,
		                                      context );
		
		Mac::ThrowOSStatus( err );
	}
	
	inline void
	//
	OTAsyncOpenEndpoint( nucleus::owned< OTConfigurationRef >  config,
	                     OTNotifyUPP                           notifier,
	                     void*                                 context )
	{
		return OTAsyncOpenEndpoint( config, NULL, notifier, context );
	}
	
}

#endif

