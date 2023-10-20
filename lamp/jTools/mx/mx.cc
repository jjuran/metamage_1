/*	=====
 *	mx.cc
 *	=====
 */

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef MAC_OS_X_VERSION_10_8
// OpenTransportProviders.h depends on FSSpec but doesn't include Files.h.
#ifndef __FILES__
#include <Files.h>
#endif
#ifndef __OPENTRANSPORTPROVIDERS__
#include <OpenTransportProviders.h>
#endif
#endif

// Standard C
#include <stdio.h>

// Standard C++
#include <algorithm>
#include <functional>

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"

// Orion
#include "Orion/Main.hh"


#ifndef MAC_OS_X_VERSION_10_8

inline bool operator<( const InetMailExchange& a, const InetMailExchange& b )
{
	return a.preference < b.preference;
}

static inline
UInt16 OTInetMailExchange( InetSvcRef         ref,
                           char*              name,
                           UInt16             num,
                           InetMailExchange*  mx )
{
	Mac::ThrowOSStatus( OTInetMailExchange( ref, name, &num, mx ) );
	
	return num;
}

#endif  // #ifndef MAC_OS_X_VERSION_10_8

namespace tool
{
	
#ifndef MAC_OS_X_VERSION_10_8
	
	static void PrintMX( const InetMailExchange& mx )
	{
		printf( "(missing) MX %d %s\n", mx.preference, mx.exchange );
		
		//N::InetHost ip = N::OTInetStringToAddress( InternetServices(), (char*) mx.exchange ).addrs[ 0 ];
		
		//Io::Out << mx.exchange << " A " << ip << "\n";
	}
	
#endif  // #ifndef MAC_OS_X_VERSION_10_8
	
	int Main( int argc, char** argv )
	{
		if ( argc < 2 )  return 1;
		
	#ifndef MAC_OS_X_VERSION_10_8
		
		const char* domain = argv[ 1 ];
		
		const int max_results = 10;  // Should be more than enough
		
		InetMailExchange results[ max_results ];
		
		UInt16 n = OTInetMailExchange( NULL,
		                               (char*) domain,
		                               max_results,
		                               &results[ 0 ] );
		
		std::sort( &results[ 0 ],
		           &results[ n ] );
		
		std::for_each( &results[ 0 ],
		               &results[ n ],
		               std::ptr_fun( PrintMX ) );
		
	#endif  // #ifndef MAC_OS_X_VERSION_10_8
		
		return 0;
	}

}
