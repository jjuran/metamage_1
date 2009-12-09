/*	=====
 *	mx.cc
 *	=====
 */

// Standard C++
#include <algorithm>
#include <vector>

// Standard C/C++
#include <cstdio>

// Nitrogen
#include "Nitrogen/OpenTransportProviders.h"

// Orion
#include "Orion/Main.hh"


inline bool operator<( const InetMailExchange& a, const InetMailExchange& b )
{
	return a.preference < b.preference;
}


namespace tool
{
	
	namespace N = Nitrogen;
	
	
	static void PrintMX( const InetMailExchange& mx )
	{
		std::printf( "(missing) MX %d %s\n", mx.preference, mx.exchange );
		
		//N::InetHost ip = N::OTInetStringToAddress( InternetServices(), (char*) mx.exchange ).addrs[ 0 ];
		
		//Io::Out << mx.exchange << " A " << ip << "\n";
	}
	
	int Main( int argc, iota::argv_t argv )
	{
		if ( argc < 2 )  return 1;
		
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
		
		return 0;
	}

}

