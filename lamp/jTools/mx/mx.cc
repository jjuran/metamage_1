/*	=====
 *	mx.cc
 *	=====
 */

// Standard C++
#include <algorithm>
#include <vector>

// OpenTransportProviders.h has evil, blind dependency on FSSpec.
#ifndef __FILES__
#include <Files.h>
#endif

// Nitrogen
#include "Nitrogen/OpenTransportProviders.h"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"


namespace N = Nitrogen;
namespace O = Orion;


inline bool operator<( const InetMailExchange& a, const InetMailExchange& b )
{
	return a.preference < b.preference;
}

static void PrintMX( const InetMailExchange& mx )
{
	Io::Out << "(missing)" << " MX " << mx.preference << " " << mx.exchange << "\n";
	
	//N::InetHost ip = N::OTInetStringToAddress( InternetServices(), (char*) mx.exchange ).addrs[ 0 ];
	
	//Io::Out << mx.exchange << " A " << ip << "\n";
}

int O::Main( int argc, argv_t argv )
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

