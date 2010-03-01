/*	================
 *	OpenTransport.hh
 *	================
 */

#ifndef CLASSICTOOLBOX_OPENTRANSPORT_HH
#define CLASSICTOOLBOX_OPENTRANSPORT_HH

// Nostalgia
#include "Nostalgia/OpenTransport.hh"

// Nitrogen
#include "Nitrogen/OpenTransport.hh"


namespace Nitrogen
{
	
	inline void InitOpenTransport()
	{
		ThrowOSStatus( ::InitOpenTransport() );
	}
	
	using ::CloseOpenTransport;
	
	inline Nucleus::Owned< EndpointRef >
	//
	OTOpenEndpoint( Nucleus::Owned< OTConfigurationRef >  config,
	                TEndpointInfo*                        info = NULL )
	{
		::OSStatus err;
		
		EndpointRef result = ::OTOpenEndpoint( config.release(),
		                                       OTOpenFlags( 0 ),
		                                       info,
		                                       &err );
		
		ThrowOSStatus( err );
		
		return Nucleus::Owned< EndpointRef >::Seize( result );
	}
	
}

#endif

