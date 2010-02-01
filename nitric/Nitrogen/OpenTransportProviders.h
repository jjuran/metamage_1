// Nitrogen/OpenTransportProviders.h
// ---------------------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2005-2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_OPENTRANSPORTPROVIDERS_H
#define NITROGEN_OPENTRANSPORTPROVIDERS_H

// Mac OS
// OpenTransportProviders.h depends on FSSpec but doesn't include Files.h.
#ifndef __FILES__
#include <Files.h>
#endif
#ifndef __OPENTRANSPORTPROVIDERS__
#include <OpenTransportProviders.h>
#endif

// Standard C++
#include <vector>

// Nucleus
#include "Nucleus/Enumeration.h"
#ifndef NUCLEUS_INITIALIZE_H
#include "Nucleus/Initialize.h"
#endif

// Nitrogen
#ifndef NITROGEN_OPENTRANSPORT_H
#include "Nitrogen/OpenTransport.h"
#endif


namespace Nitrogen
{
	
	class DefaultInternetServicesPath {};
	
}

#ifdef kDefaultInternetServicesPath
#undef kDefaultInternetServicesPath

// This definition is useless to clients because OTOpenInternetServices[InContext]() takes
// Nucleus::Owned< OTConfigurationRef >, not OTConfigurationRef.  You can't get there from here.
// Instead, we define a new class and overload on that.

//static const OTConfigurationRef kDefaultInternetServicesPath = (OTConfigurationRef)-3L;

extern const Nitrogen::DefaultInternetServicesPath kDefaultInternetServicesPath;

#endif

// However, we still need to use it internally, so we define it in OpenTransportProviders.cp.


#ifdef SET_TOS
#undef SET_TOS

inline unsigned char SET_TOS( unsigned char prec, unsigned char tos )
{
	return (0x7 & prec) << 5  |  (0x1c & tos);
}

#endif

#ifdef OTOpenInternetServices
#undef OTOpenInternetServices
#endif


namespace Nitrogen
{
	
	enum InetPort
	{
		kInetPort_Max = Nucleus::Enumeration_Traits< ::InetPort >::max
	};
	
	// Default InetHost is kOTAnyInetAddress (i.e. 0).
	// Alternative choice is 0xFFFFFFFF (no address).
	
	class InetHost
	{
		private:
			union
			{
				::InetHost address;
				unsigned char octets[ 4 ];  // This makes debugging easier
			};
		
		public:
			InetHost() : address( kOTAnyInetAddress )  {}
			
			InetHost( ::InetHost addr ) : address( addr )  {}
			
			InetHost( unsigned char a,
			          unsigned char b,
			          unsigned char c,
			          unsigned char d ) : address( a << 24 | b << 16 | c << 8 | d )  {}
			
			static InetHost Make( ::InetHost addr )  { return InetHost( addr ); }
			
			::InetHost Get() const  { return address; }
			operator ::InetHost() const  { return Get(); }
	};
	
	using ::InetSvcRef;
	
}

namespace Nucleus
{
	
	template <> struct Disposer_Traits< Nitrogen::InetSvcRef > : Disposer_Traits< Nitrogen::ProviderRef > {};
	
}

namespace Nitrogen
{
	
	using ::InetAddress;
	
	typedef std::string InetDomainName;
	
	using ::InetHostInfo;
	using ::InetSysInfo;
	using ::InetMailExchange;
	
	InetAddress& OTInitInetAddress( InetAddress& addr, InetPort port, InetHost host );
	
}

namespace Nucleus
{
	
	template <>
	struct Initializer< Nitrogen::InetAddress >
	{
		Nitrogen::InetAddress& operator()( Nitrogen::InetAddress&  addr,
		                                   Nitrogen::InetPort      port,
		                                   Nitrogen::InetHost      host )
		{
			return Nitrogen::OTInitInetAddress( addr, port, host );
		}
	};
	
}

namespace Nitrogen
{
	
	#pragma mark -
	#pragma mark ¥ IP address encoding ¥
	
	InetHost OTInetStringToHost( const char* str );
	
	inline InetHost OTInetStringToHost( const std::string& str )
	{
		return OTInetStringToHost( str.c_str() );
	}
	
	std::string OTInetHostToString( InetHost host );
	
}

namespace Nucleus
{
	template <>
	struct Converter< Nitrogen::InetHost, const char* > : std::unary_function< const char*, Nitrogen::InetHost >
	{
		Nitrogen::InetHost operator()( const char* input ) const
		{
			return Nitrogen::OTInetStringToHost( input );
		}
	};
	
	template <>
	struct Converter< Nitrogen::InetHost, std::string > : std::unary_function< std::string, Nitrogen::InetHost >
	{
		Nitrogen::InetHost operator()( const std::string& input ) const
		{
			return Nitrogen::OTInetStringToHost( input );
		}
	};
	
	template <>
	struct Converter< std::string, Nitrogen::InetHost > : std::unary_function< Nitrogen::InetHost, std::string >
	{
		std::string operator()( Nitrogen::InetHost input ) const
		{
			return Nitrogen::OTInetHostToString( input );
		}
	};
	
}

namespace Nitrogen
{
	
	#pragma mark -
	#pragma mark ¥ DNR ¥
	
	Nucleus::Owned< InetSvcRef > OTOpenInternetServicesInContext( Nucleus::Owned< OTConfigurationRef >  cfig,
	                                                              OTClientContextPtr                    clientContext = NULL );
	
	Nucleus::Owned< InetSvcRef > OTOpenInternetServicesInContext( DefaultInternetServicesPath  /**/,
	                                                              OTClientContextPtr           clientContext = NULL );
	
	InetHostInfo& OTInetStringToAddress( InetSvcRef     ref,
	                                     char*          name,
	                                     InetHostInfo&  hInfo );
	
	inline InetHostInfo OTInetStringToAddress( InetSvcRef ref, char* name )
	{
		InetHostInfo result;
		
		return OTInetStringToAddress( ref, name, result );
	}
	
	InetDomainName OTInetAddressToName( InetSvcRef ref, InetHost addr );
	
	// OTInetSysInfo
	
	void OTInetMailExchange( InetSvcRef                        ref,
	                         char*                             name,
	                         std::vector< InetMailExchange >&  mx );
	
}

#endif

