/*	=========================
 *	OpenTransportProviders.cp
 *	=========================
 */

#include "ClassicToolbox/OpenTransportProviders.h"


namespace Nitrogen
{
	
	namespace Detail
	{
		
		static const OTConfigurationRef kDefaultInternetServicesPath = (OTConfigurationRef)-3L;
		
	}
	
	
	Nucleus::Owned< InetSvcRef >
	//
	OTOpenInternetServices( Nucleus::Owned< OTConfigurationRef > config )
	{
		::OSStatus err;
		
		InetSvcRef result = ::OTOpenInternetServices( config.Release(),
		                                              OTOpenFlags( 0 ),
		                                              &err );
		
		ThrowOSStatus( err );
		
		return Nucleus::Owned< InetSvcRef >::Seize( result );
	}
	
	Nucleus::Owned< InetSvcRef >
	//
	OTOpenInternetServices( DefaultInternetServicesPath /**/ )
	{
		::OSStatus err;
		
		InetSvcRef result = ::OTOpenInternetServices( Detail::kDefaultInternetServicesPath,
		                                              OTOpenFlags( 0 ),
		                                              &err );
		
		ThrowOSStatus( err );
		
		return Nucleus::Owned< InetSvcRef >::Seize( result );
	}
	
}

