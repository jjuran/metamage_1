/*	=====
 *	mx.cc
 *	=====
 */

// Standard C++
#include <algorithm>
#include <string>

// Nitrogen
#include "Nitrogen/OpenTransportProviders.h"

// Resolver
//#include "Resolver.hh"

// Highway
//#include "HighwayDNR.hh"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"
#include "SystemCalls.hh"


namespace N = Nitrogen;
//namespace HW = Highway;
namespace O = Orion;

using std::string;
using std::vector;

//using Resolver::MX;


/*
static void PrintMX( const MX& mx )
{
	Io::Out << mx.name << " MX " << mx.preference << " " << mx.mxHost << "\n";
	
	HW::IPAddress ipAddr = HW::DNSResolve( mx.mxHost );
	
	string ipStr = HW::EncodeIP( ipAddr );
	
	std::printf( "%s A %s\n",
	             mx.mxHost.c_str(),
	             ipStr.c_str() );
}
*/

inline bool operator<( const InetMailExchange& a, const InetMailExchange& b )
{
	return a.preference < b.preference;
}

static void PrintMX( const InetMailExchange& mx )
{
	Io::Out << "(missing)" << " MX " << mx.preference << " " << mx.exchange << "\n";
	
	N::InetHost ip = N::OTInetStringToAddress( InternetServices(), (char*)mx.exchange ).addrs[ 0 ];
	
	/*
	InetHostInfo hInfo;
	OSStatus err = O::OTInetStringToAddress( (char*)mx.exchange, &hInfo );
	N::ThrowOSStatus( err );
	
	N::InetHost ip = hInfo.addrs[ 0 ];
	*/
	
	Io::Out << mx.exchange << " A " << ip << "\n";
}

int O::Main(int argc, const char *const argv[])
{
	if ( argc < 2 )  return 1;
	
	const char* domain = argv[ 1 ];
	
	//vector< MX > results = Resolver::LookupMX( domain );
	
	std::vector< InetMailExchange > results;
	
	results.resize( 10 );  // Should be more than enough
	
	N::OTInetMailExchange( InternetServices(),
	                       (char*)domain,
	                       results );
	
	/*
	UInt16 num = 10;
	N::ThrowOSStatus( O::OTInetMailExchange( (char*)domain, &num, &results.front() ) );
	results.resize( num );
	*/
	
	std::sort( results.begin(),
	           results.end() );
	
	std::for_each( results.begin(),
	               results.end(),
	               std::ptr_fun( PrintMX ) );
	
	return 0;
}

