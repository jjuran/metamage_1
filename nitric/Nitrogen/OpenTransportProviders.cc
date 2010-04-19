// Nitrogen/OpenTransportProviders.cc
// ----------------------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2005-2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "Nitrogen/OpenTransportProviders.hh"


using Nitrogen::DefaultInternetServicesPath;

const DefaultInternetServicesPath kDefaultInternetServicesPath = DefaultInternetServicesPath();

namespace Nitrogen
{
	
	NUCLEUS_REQUIRE_ERRORS_STATIC( OpenTransport )
	
	
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
	
	nucleus::string OTInetHostToString( InetHost host )
	{
		char result[ 16 ];
		
		::OTInetHostToString( host, result );
		
		return result;
	}
	
	nucleus::owned< InetSvcRef > OTOpenInternetServicesInContext( nucleus::owned< OTConfigurationRef >  cfig,
	                                                              OTClientContextPtr                    clientContext )
	{
		::OSStatus err;
		
		InetSvcRef result = ::OTOpenInternetServicesInContext( cfig.release(),
		                                                       OTOpenFlags( 0 ),
		                                                       &err,
		                                                       clientContext );
		
		ThrowOSStatus( err );
		
		return nucleus::owned< InetSvcRef >::seize( result );
	}
	
	nucleus::owned< InetSvcRef > OTOpenInternetServicesInContext( DefaultInternetServicesPath  /**/,
	                                                              OTClientContextPtr           clientContext )
	{
		::OSStatus err;
		
		InetSvcRef result = ::OTOpenInternetServicesInContext( Detail::kDefaultInternetServicesPath,
		                                                       OTOpenFlags( 0 ),
		                                                       &err,
		                                                       clientContext );
		
		ThrowOSStatus( err );
		
		return nucleus::owned< InetSvcRef >::seize( result );
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

