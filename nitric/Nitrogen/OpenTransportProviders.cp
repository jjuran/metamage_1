// ================
// OpenTransport.cp
// ================

#ifndef NITROGEN_OPENTRANSPORTPROVIDERS_H
#include "Nitrogen/OpenTransportProviders.h"
#endif


namespace Nitrogen
{
	
	static OpenTransportErrorsRegistrationDependency gOpenTransportErrorsRegistrationDependency;
	
	
	namespace Detail
	{
		
		static const OTConfigurationRef kDefaultInternetServicesPath = (OTConfigurationRef)-3L;
		
	}
	
	
	InetAddress& OTInitInetAddress( InetAddress&  addr,
	                                InetPort      port,
	                                InetHost      host )
	{
		::OTInitInetAddress( &addr, port, host );
		
		return addr;
	}
	
	InetHost OTInetStringToHost( const char* str )
	{
		::InetHost result;
		
		ThrowOSStatus( ::OTInetStringToHost( str, &result ) );
		
		return result;
	}
	
	std::string OTInetHostToString( InetHost host )
	{
		char result[ 16 ];
		
		::OTInetHostToString( host, result );
		
		return result;
	}
	
	Nucleus::Owned< InetSvcRef > OTOpenInternetServicesInContext( Nucleus::Owned< OTConfigurationRef >  cfig,
	                                                              OTClientContextPtr                    clientContext )
	{
		::OSStatus err;
		
	#if TARGET_API_MAC_CARBON
		
		InetSvcRef result = ::OTOpenInternetServicesInContext( cfig.Release(),
		                                                       OTOpenFlags( 0 ),
		                                                       &err,
		                                                       clientContext );
		
	#else
		
		InetSvcRef result = ::OTOpenInternetServices( cfig.Release(),
		                                              OTOpenFlags( 0 ),
		                                              &err );
		
	#endif
		
		ThrowOSStatus( err );
		
		return Nucleus::Owned< InetSvcRef >::Seize( result );
	}
	
	Nucleus::Owned< InetSvcRef > OTOpenInternetServicesInContext( DefaultInternetServicesPath  /**/,
	                                                              OTClientContextPtr           clientContext )
	{
		::OSStatus err;
		
	#if TARGET_API_MAC_CARBON
		
		InetSvcRef result = ::OTOpenInternetServicesInContext( Detail::kDefaultInternetServicesPath,
		                                                       OTOpenFlags( 0 ),
		                                                       &err,
		                                                       clientContext );
		
	#else
		
		InetSvcRef result = ::OTOpenInternetServices( Detail::kDefaultInternetServicesPath,
		                                              OTOpenFlags( 0 ),
		                                              &err );
		
	#endif
		
		ThrowOSStatus( err );
		
		return Nucleus::Owned< InetSvcRef >::Seize( result );
	}
	
	InetHostInfo& OTInetStringToAddress( InetSvcRef     ref,
	                                     char*          name,
	                                     InetHostInfo&  hInfo )
	{
		ThrowOSStatus( ::OTInetStringToAddress( ref, name, &hInfo ) );
		
		return hInfo;
	}
	
	InetDomainName OTInetAddressToName( InetSvcRef ref, InetHost addr )
	{
		::InetDomainName result;
		
		ThrowOSStatus( ::OTInetAddressToName( ref, addr, result ) );
		
		return result;
	}
	
	void OTInetMailExchange( InetSvcRef                        ref,
	                         char*                             name,
	                         std::vector< InetMailExchange >&  mx )
	{
		UInt16 num = mx.size();
		
		ThrowOSStatus( ::OTInetMailExchange( ref, name, &num, &mx.front() ) );
		
		mx.resize( num );
	}
	
}

