/*	=========================
 *	OpenTransportProviders.hh
 *	=========================
 */

#ifndef CLASSICTOOLBOX_OPENTRANSPORTPROVIDERS_HH
#define CLASSICTOOLBOX_OPENTRANSPORTPROVIDERS_HH

// Nostalgia
#include "Nostalgia/OpenTransportProviders.hh"

// Nitrogen
#include "Nitrogen/OpenTransportProviders.hh"


namespace Nitrogen
{
	
	namespace Detail
	{
		
		static const OTConfigurationRef kDefaultInternetServicesPath = (OTConfigurationRef)-3L;
		
	}
	
	
	inline Nucleus::Owned< InetSvcRef >
	//
	OTOpenInternetServices( Nucleus::Owned< OTConfigurationRef > config )
	{
		::OSStatus err;
		
		InetSvcRef result = ::OTOpenInternetServices( config.release(),
		                                              OTOpenFlags( 0 ),
		                                              &err );
		
		ThrowOSStatus( err );
		
		return Nucleus::Owned< InetSvcRef >::Seize( result );
	}
	
	inline Nucleus::Owned< InetSvcRef >
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

#endif

