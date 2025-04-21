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

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"


#if CONFIG_OPEN_TRANSPORT_HEADERS

using Nitrogen::DefaultInternetServicesPath;

const DefaultInternetServicesPath kDefaultInternetServicesPath = DefaultInternetServicesPath();

namespace Nitrogen
{
	
	using Mac::ThrowOSStatus;
	
	
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
		
		InetSvcRef result = ::OTOpenInternetServicesInContext( cfig,
		                                                       OTOpenFlags( 0 ),
		                                                       &err,
		                                                       clientContext );
		
		ThrowOSStatus( err );
		
		cfig.release();
		
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
	
	UInt16 OTInetMailExchange( InetSvcRef         ref,
	                           char*              name,
	                           UInt16             num,
	                           InetMailExchange*  mx )
	{
		ThrowOSStatus( ::OTInetMailExchange( ref, name, &num, mx ) );
		
		return num;
	}
	
}

#else  // #if CONFIG_OPEN_TRANSPORT_HEADERS

int dummy;

#endif  // #if CONFIG_OPEN_TRANSPORT_HEADERS
