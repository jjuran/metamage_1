/*	===========
 *	sendmail.cc
 *	===========
 */

// Standard C/C++
#include <cctype>

// Standard C++
#include <algorithm>
#include <string>
#include <vector>

// POSIX
#include "netinet/in.h"
#include "sys/socket.h"
#include "unistd.h"

// Io
#include "io/slurp.hh"

// Nitrogen
#include "Nitrogen/OpenTransportProviders.h"

// Nitrogen Extras / Iteration
#include "Iteration/FSContents.h"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"

// Io
#include "Io/Files.hh"
#include "Io/MakeHandle.hh"

// Arcana / SMTP
#include "SMTP.hh"

// Kerosene
#include "SystemCalls.hh"

// Orion
#include "Orion/GetOptions.hh"
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"


namespace N = Nitrogen;
namespace NN = Nucleus;
namespace O = Orion;


//using Resolver::MX;


inline bool operator<( const InetMailExchange& a, const InetMailExchange& b )
{
	return a.preference < b.preference;
}


static std::string gRelay;

struct BadEmailAddress : N::ParamErr {};


static N::FSDirSpec QueueDirectory()
{
	return N::RootDirectory( N::BootVolume() ) << "j"
	                                           << "var"
	                                           << "spool"
	                                           << "jmail"
	                                           << "queue";
}


static std::string DomainFromEmailAddress( const std::string& emailAddr )
{
	std::string::size_type at = emailAddr.find( '@' );
	
	if ( at >= emailAddr.size() - 1 )
	{
		throw BadEmailAddress();
	}
	
	return emailAddr.substr( at + 1, emailAddr.find( '>' ) - (at + 1) );
}

/*
static std::string ResolverLookup( const std::string& domain )
{
	try
	{
		std::vector< MX > results = Resolver::LookupMX( domain );
		//ASSERT(!results.empty());
		
		std::sort( results.begin(),
		           results.end() );
		
		return results.front().mxHost;
	}
	catch ( Resolver::ResolverError error )
	{
		switch ( error.Errno() )
		{
			case NO_DATA:
				// No MX data.  No problem, we'll try A instead.
				break;
			
			default:
				throw;
		}
	}
	
	return "";
}
*/

static std::string OTLookup( const std::string& domain )
{
	std::vector< InetMailExchange > results;
	
	results.resize( 10 );  // Should be more than enough
	
	N::OTInetMailExchange( InternetServices(),
	                       (char*) domain.c_str(),
	                       results );
	
	/*
	UInt16 num = 10;
	N::ThrowOSStatus( O::OTInetMailExchange( (char*)domain.c_str(), &num, &results.front() ) );
	results.resize( num );
	*/
	
	std::sort( results.begin(),
	           results.end() );
	
	return results.front().exchange;
}

static void Relay( const std::string&  returnPath,
                   const std::string&  forwardPath,
                   const FSSpec&       messageFile )
{
	if ( forwardPath == "" )
	{
		// This will be the case if the dest file's data fork was empty,
		// which will be true of an Icon file.
		// The file will be deleted after we return.
		return;
	}
	
	Io::Out << "Relaying from "
	        << returnPath
	        << " to "
	        << forwardPath
	        << "\n";
	
	std::string smtpServer = gRelay;
	
	if ( !gRelay.empty() )
	{
		Io::Out << "Using relay " << gRelay << ".\n";
	}
	else
	{
		std::string rcptDomain = DomainFromEmailAddress( forwardPath );
		
		//smtpServer = ResolverLookup( rcptDomain );
		smtpServer = OTLookup( rcptDomain );
		
		if ( !smtpServer.empty() )
		{
			Io::Out << "MX for " << rcptDomain << " is " << smtpServer << ".\n";
		}
		else
		{
			smtpServer = rcptDomain;
			Io::Out << "Using domain " << rcptDomain << " as server.\n";
		}
		
	}
	
	int smtpPort = 25;
	
	// Get the IP address of the SMTP server.
	//HW::IPAddress ip = HW::DNSResolve( smtpServer );
	
	N::InetHost ip = N::OTInetStringToAddress( InternetServices(),
	                                           (char*) smtpServer.c_str() ).addrs[ 0 ];
	
	/*
	InetHostInfo hInfo;
	N::ThrowOSStatus( O::OTInetStringToAddress( (char*)smtpServer.c_str(), &hInfo ) );
	N::InetHost ip = hInfo.addrs[ 0 ];
	*/
	
	Io::Out << "Address of " << smtpServer << " is " << ip << ".\n";
	
	// Make a new socket
	//N::Owned< HW::FileDescriptor > theSocket( HW::NewSocket() );
	
	int sock = socket( PF_INET, SOCK_STREAM, INET_TCP );
	
	// and connect to the server.  This could fail, thanks to a bunch of Cox.
	//HW::ConnectSocket( theSocket, N::Make< HW::PortAddress >( ip, smtpPort ) );
	
	struct sockaddr_in inetAddress;
	
	inetAddress.sin_family = AF_INET;
	inetAddress.sin_port = smtpPort;
	inetAddress.sin_addr.s_addr = ip;
	
	int result = connect( sock, (const sockaddr*) &inetAddress, sizeof (sockaddr_in) );
	
	Io::Handle io = Io::MakeHandleFromCast< Io::FD_Details >( sock );
	
	SMTP::Client::Session smtpSession( io );
	
	NN::Owned< N::FSFileRefNum > messageStream = io::open_for_reading( messageFile );
	
	//smtpSession.Hello      ( HW::GetHostname() );
	smtpSession.Hello      ( "hostname"  );
	smtpSession.MailFrom   ( returnPath  );
	smtpSession.RecipientTo( forwardPath );
	
	smtpSession.BeginData();
	
	while ( true )
	{
		enum { kDataSize = 4096 };
		
		char data[ kDataSize ];
		
		try
		{
			int bytes = io::read( messageStream, data, kDataSize );
			io.Write( data, bytes );
		}
		catch ( const io::end_of_input& )
		{
			break;
		}
	}
	
	smtpSession.EndData();
	smtpSession.Quit();
}

static bool IsControlChar( char c )
{
	return std::iscntrl( c );
}

static std::string ReadOneLiner( N::FSFileRefNum fileH )
{
	std::string contents;
	
	contents.resize( N::GetEOF( fileH ) );
	
	io::read( fileH, &contents[0], contents.size() );
	
	const std::string::const_iterator end_of_first_line = std::find_if( contents.begin(),
	                                                                    contents.end(),
	                                                                    std::ptr_fun( IsControlChar ) );
	
	const std::size_t length_of_first_line = end_of_first_line - contents.begin();
	
	contents.resize( length_of_first_line );
	
	return contents;
}

static std::string ReadOneLiner( const FSSpec& file )
{
	return ReadOneLiner( io::open_for_reading( file ) );
}


class Transmitter
{
	private:
		std::string  itsReturnPath;
		FSSpec       itsMessageFile;
	
	public:
		Transmitter( const std::string&  returnPath,
		             const FSSpec&       message ) : itsReturnPath ( returnPath ),
		                                             itsMessageFile( message    )
		{}
		
		void operator()( const FSSpec& destFile );
};

void Transmitter::operator()( const FSSpec& destFile )
{
	try
	{
		using N::fsRdWrPerm;
		
		// destFile serves as a lock on this destination
		Relay( itsReturnPath,
		       ReadOneLiner( N::FSpOpenDF( destFile, fsRdWrPerm ) ),
		       itsMessageFile );
		
		N::FSpDelete( destFile );
	}
	catch ( ... )
	{
		
	}
}


static void ProcessMessage( const FSSpec& msgFolderItem )
{
	if ( !N::FSpTestDirectoryExists( msgFolderItem ) )  return;  // Icon files, et al
	
	N::FSDirSpec msgFolder = NN::Convert< N::FSDirSpec >( msgFolderItem );
	
	FSSpec       message    = msgFolder & "Message";
	FSSpec       returnPath = msgFolder & "Return-Path";
	
	N::FSDirSpec destFolder = msgFolder << "Destinations";
	
	std::for_each( N::FSContents( destFolder ).begin(),
	               N::FSContents( destFolder ).end(),
	               Transmitter( ReadOneLiner( returnPath ),
	                            message ) );
	
	N::FSpDelete( NN::Convert< FSSpec >( destFolder ) );  // this fails if destinations remain
	N::FSpDelete( returnPath );
	N::FSpDelete( message    );
	N::FSpDelete( NN::Convert< FSSpec >( msgFolder )  );
}


int O::Main(int argc, const char *const argv[])
{
	if ( argc >= 3 )
	{
		if ( argv[ 1 ] == std::string( "--relay" ) )
		{
			gRelay = argv[ 2 ];
		}
	}
	
	N::FSDirSpec queue = QueueDirectory();
	
	std::for_each( N::FSContents( queue ).begin(),
	               N::FSContents( queue ).end(),
	               std::ptr_fun( ProcessMessage ) );
	
	return 0;
}

