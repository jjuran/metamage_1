/*	=====
 *	mx.cc
 *	=====
 */

// Standard C
#include <stdio.h>

// Standard C++
#include <algorithm>
#include <vector>

// Nitrogen
#include "Nitrogen/OpenTransportProviders.hh"

// Orion
#include "Orion/Main.hh"


#ifndef MAC_OS_X_VERSION_10_8

inline bool operator<( const InetMailExchange& a, const InetMailExchange& b )
{
	return a.preference < b.preference;
}

#endif  // #ifndef MAC_OS_X_VERSION_10_8

namespace tool
{
	
#ifndef MAC_OS_X_VERSION_10_8
	
	namespace N = Nitrogen;
	
	
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
		
		std::vector< InetMailExchange > results;
		
		results.resize( 10 );  // Should be more than enough
		
		N::OTInetMailExchange( NULL,
		                       (char*) domain,
		                       results );
		
		std::sort( results.begin(),
		           results.end() );
		
		std::for_each( results.begin(),
		               results.end(),
		               std::ptr_fun( PrintMX ) );
		
	#endif  // #ifndef MAC_OS_X_VERSION_10_8
		
		return 0;
	}

}
