/*	================
 *	OpenTransport.cp
 *	================
 */

#include "ClassicToolbox/OpenTransport.h"


namespace Nitrogen
{
	
	Nucleus::Owned< EndpointRef >
	//
	OTOpenEndpoint( Nucleus::Owned< OTConfigurationRef >  config,
	                TEndpointInfo*                        info )
	{
		::OSStatus err;
		
		EndpointRef result = ::OTOpenEndpoint( config.Release(),
		                                       OTOpenFlags( 0 ),
		                                       info,
		                                       &err );
		
		ThrowOSStatus( err );
		
		return Nucleus::Owned< EndpointRef >::Seize( result );
	}
	
}

