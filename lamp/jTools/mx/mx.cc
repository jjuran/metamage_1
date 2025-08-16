/*	=====
 *	mx.cc
 *	=====
 */

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// mac-config
#include "mac_config/open-transport.hh"

// Mac OS
#if CONFIG_OPEN_TRANSPORT_HEADERS
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

// Orion
#include "Orion/Main.hh"


#pragma exceptions off


#if CONFIG_OPEN_TRANSPORT_HEADERS

inline bool operator<( const InetMailExchange& a, const InetMailExchange& b )
{
	return a.preference < b.preference;
}

static inline
OSStatus OTInetMailExchange( InetSvcRef         ref,
                             char*              name,
                             UInt16             num,
                             InetMailExchange*  mx )
{
	OSStatus err = OTInetMailExchange( ref, name, &num, mx );
	
	return err ? err : num;
}

#endif  // #if CONFIG_OPEN_TRANSPORT_HEADERS

namespace tool
{
	
	int Main( int argc, char** argv )
	{
		if ( argc < 2 )  return 1;
		
	#if CONFIG_OPEN_TRANSPORT_HEADERS
		
		const char* domain = argv[ 1 ];
		
		const int max_results = 10;  // Should be more than enough
		
		InetMailExchange results[ max_results ];
		
		OTResult n = OTInetMailExchange( NULL,
		                                 (char*) domain,
		                                 max_results,
		                                 &results[ 0 ] );
		
		if ( n < 0 )
		{
			fprintf( stderr, "mx: %s: OSStatus %ld\n", domain, n );
			
			return 1;
		}
		
		std::sort( &results[ 0 ],
		           &results[ n ] );
		
		for ( int i = 0;  i < n;  ++i )
		{
			const InetMailExchange& mx = results[ i ];
			
			printf( "(missing) MX %d %s\n", mx.preference, mx.exchange );
		}
		
	#endif  // #if CONFIG_OPEN_TRANSPORT_HEADERS
		
		return 0;
	}

}
