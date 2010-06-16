/*	========
 *	htget.cc
 *	========
 */

// Standard C/C++
#include <cerrno>
#include <cstdlib>

// POSIX
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>

// Iota
#include "iota/decimal.hh"
#include "iota/strings.hh"

// poseven
#include "poseven/bundles/inet.hh"
#include "poseven/extras/pump.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/socket.hh"
#include "poseven/functions/write.hh"
#include "poseven/types/exit_t.hh"

// Arcana
#include "HTTP.hh"

// Orion
#include "Orion/get_options.hh"
#include "Orion/Main.hh"


namespace tool
{
	
	namespace n = nucleus;
	namespace p7 = poseven;
	namespace o = orion;
	
	
	static bool ParseURL( const plus::string& url,
	                      plus::string& outURLScheme, 
	                      plus::string& outHostname,
	                      plus::string& outPort,
	                      plus::string& outURLpath )
	{
		std::size_t colonSlashSlash = url.find( "://" );
		
		if ( colonSlashSlash == url.npos )
		{
			return false;
		}
		
		outURLScheme = url.substr( 0, colonSlashSlash );
		
		std::size_t hostnameStart = colonSlashSlash + 3;
		std::size_t slash = url.find( "/", hostnameStart );
		std::size_t colon = url.find( ":", hostnameStart );
		std::size_t hostnameEnd = std::min( slash, colon );
		
		outHostname = url.substr( hostnameStart, hostnameEnd - hostnameStart );
		
		if ( colon < slash )
		{
			outPort = url.substr( colon + 1, slash - (colon + 1) );
		}
		
		outURLpath = (slash == url.npos) ? plus::string( "/" ) : url.substr( slash, url.npos );
		
		return true;
	}
	
	
	static const char* DocName( const plus::string& urlPath )
	{
		std::size_t lastSlash = urlPath.find_last_of( "/" );
		
		// Skip the slash.
		// If there wasn't one, then lastSlash == string::npos == 0xFFFFFFFF == -1.
		// Adding one then yields zero, which is exactly what we want.
		
		return urlPath.c_str() + lastSlash + 1;
	}
	
	
	static p7::in_addr_t ResolveHostname( const char* hostname )
	{
		hostent* hosts = gethostbyname( hostname );
		
		if ( !hosts || h_errno )
		{
			plus::var_string message = "Domain name lookup failed: ";
			
			message += iota::inscribe_decimal( h_errno );
			message += "\n";
			
			p7::write( p7::stderr_fileno, message );
			
			throw p7::exit_failure;
		}
		
		in_addr addr = *(in_addr*) hosts->h_addr;
		
		return p7::in_addr_t( addr.s_addr );
	}
	
	
	static void receive_document( const plus::string&  partial_content,
	                              p7::fd_t             http_server,
	                              p7::fd_t             document_destination )
	{
		p7::write( document_destination, partial_content );
		
		p7::pump( http_server, document_destination );
	}
	
	int Main( int argc, char** argv )
	{
		bool sendHEADRequest = false;
		bool dumpHeader      = false;
		bool saveToFile      = false;
		
		const char* defaultOutput = "/dev/fd/1";
		
		const char* outputFile = defaultOutput;
		
		o::bind_option_to_variable( "-i", dumpHeader      );
		o::bind_option_to_variable( "-I", sendHEADRequest );
		o::bind_option_to_variable( "-o", outputFile      );
		o::bind_option_to_variable( "-O", saveToFile      );
		
		o::alias_option( "-i", "--headers" );
		o::alias_option( "-i", "--include" );
		
		o::alias_option( "-I", "--head" );
		
		o::alias_option( "-O", "--remote-name" );
		o::alias_option( "-O", "--save"        );
		
		o::get_options( argc, argv );
		
		char const *const *freeArgs = o::free_arguments();
		
		const size_t n_args = o::free_argument_count();
		
		if ( n_args == 0 )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "htget: Usage:  htget <url>\n" ) );
			
			return EXIT_FAILURE;
		}
		
		const char* method = "GET";
		
		const bool expecting_content = !sendHEADRequest;
		
		if ( sendHEADRequest )
		{
			dumpHeader = true;
			method = "HEAD";
		}
		
		if ( !expecting_content  &&  (saveToFile  ||  outputFile != defaultOutput) )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "htget: Can't save null document to file\n" ) );
			
			return EXIT_FAILURE;
		}
		
		plus::string scheme;
		plus::string hostname;
		plus::string urlPath;
		plus::string portStr;
		
		p7::in_port_t default_port = p7::in_port_t( 0 );
		
		bool parsed = ParseURL( freeArgs[ 0 ], scheme, hostname, portStr, urlPath );
		
		// FIXME:  Eliminate . and .. from urlPath
		
		if ( saveToFile )
		{
			outputFile = DocName( urlPath );
		}
		
		bool outputIsToFile = outputFile != defaultOutput;
		
		if ( scheme == "http" )
		{
			default_port = p7::in_port_t( 80 );
		}
		else
		{
			plus::string message = "Unsupported scheme '" + scheme + "'.\n";
			
			p7::write( p7::stderr_fileno, message );
			
			return 2;
		}
		
		p7::in_port_t port = default_port;
		
		if ( !portStr.empty() )
		{
			port = p7::in_port_t( iota::parse_unsigned_decimal( portStr.c_str() ) );
		}
		
		p7::in_addr_t ip = ResolveHostname( hostname.c_str() );
		
		plus::string message_header =   HTTP::RequestLine( method, urlPath.c_str(), urlPath.size() )
		                              + HTTP::HeaderFieldLine( "Host", hostname )
		                              + "\r\n";
		
		n::owned< p7::fd_t > http_server = p7::connect( ip, port );
		
		p7::write( http_server, message_header );
		
		p7::open_flags_t create_flags = outputIsToFile ? p7::o_creat | p7::o_excl : p7::open_flags_t();
		
		HTTP::ResponseReceiver response;
		
		response.ReceiveHeader( http_server );
		
		if ( dumpHeader )
		{
			p7::write( p7::stdout_fileno, response.GetMessageStream() );
		}
		
		if ( expecting_content )
		{
			receive_document( response.GetPartialContent(),
			                  http_server,
			                  p7::open( outputFile, p7::o_wronly | create_flags ) );
		}
		
		return 0;
	}
	
}

