/*	========
 *	htget.cc
 *	========
 */

// POSIX
#include <fcntl.h>
#include <netdb.h>

// Standard C
#include <errno.h>
#include <stdlib.h>

// Iota
#include "iota/strings.hh"

// command
#include "command/get_option.hh"

// gear
#include "gear/find.hh"
#include "gear/inscribe_decimal.hh"

// poseven
#include "poseven/bundles/inet.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/perror.hh"
#include "poseven/functions/read.hh"
#include "poseven/functions/write.hh"

// Arcana
#include "HTTP.hh"

// Orion
#include "Orion/Main.hh"


#ifdef __RELIX__
#define DEFAULT_USER_AGENT  "htget (a MacRelix program)"
#else
#define DEFAULT_USER_AGENT  "htget"
#endif


using namespace command::constants;

enum
{
	Option_HEAD = 'I',
	Option_save = 'O',
	Option_dump = 'i',
	Option_file = 'o',
	
	Option_last_byte = 255,
	
	Option_progress,
};

static command::option options[] =
{
	{ "head", Option_HEAD },
	{ "save", Option_save },
	
	{ "remote-name", Option_save },
	
	{ "headers", Option_dump },
	{ "include", Option_dump },
	
	{ "dump-progress", Option_progress },
	
	{ "", Option_file, Param_required },
	
	{ NULL }
};

static const char* const defaultOutput = "/dev/fd/1";

static const char* outputFile = defaultOutput;

static bool sendHEADRequest = false;
static bool dumpHeader      = false;
static bool saveToFile      = false;

static bool dumping_progress = false;

static char* const* get_options( char* const* argv )
{
	++argv;  // skip arg 0
	
	short opt;
	
	while ( (opt = command::get_option( &argv, options )) )
	{
		switch ( opt )
		{
			case Option_HEAD:
				sendHEADRequest = true;
				break;
			
			case Option_save:
				saveToFile = true;
				break;
			
			case Option_dump:
				dumpHeader = true;
				break;
			
			case Option_progress:
				dumping_progress = true;
				break;
			
			case Option_file:
				outputFile = command::global_result.param;
				break;
			
			default:
				abort();
		}
	}
	
	return argv;
}


namespace tool
{
	
	namespace n = nucleus;
	namespace p7 = poseven;
	
	
	static size_t content_length = 0;
	
	static plus::string content_length_string;
	
	static void dump_progress( size_t content_bytes_received )
	{
		plus::var_string dump = gear::inscribe_unsigned_decimal( content_bytes_received );
		
		dump += '/';
		
		dump += content_length_string;
		
		dump += '\n';
		
		p7::write( p7::stdout_fileno, dump );
	}
	
	static const char* ParseURL( const char*    url,
	                             plus::string&  outURLScheme, 
	                             plus::string&  outHostname,
	                             plus::string&  outPort )
	{
		const char* colon_slash_slash = strstr( url, "://" );
		
		if ( colon_slash_slash == NULL )
		{
			return NULL;
		}
		
		outURLScheme.assign( url, colon_slash_slash );
		
		const char* hostname = colon_slash_slash + STRLEN( "://" );
		
		const char* end = url + strlen( url );
		
		const char* slash = std::find( hostname, end, '/' );
		const char* colon = std::find( hostname, end, ':' );
		
		const char* hostname_end = std::min( slash, colon );
		
		outHostname.assign( hostname, hostname_end );
		
		if ( colon < slash )
		{
			outPort.assign( colon + 1, slash );
		}
		
		return slash != end ? slash : "/";
	}
	
	
	static inline const char* DocName( const char* url_path, std::size_t length )
	{
		return gear::find_last_match( url_path, length, '/', url_path - 1 ) + 1;
	}
	
	
	static void receive_document( const plus::string&  partial_content,
	                              p7::fd_t             http_server,
	                              p7::fd_t             document_destination )
	{
		size_t n_written = p7::write( document_destination, partial_content );
		
		const size_t buffer_size = 4096;
		
		char buffer[ buffer_size ];
		
		while ( const ssize_t n_read = p7::read( http_server, buffer, buffer_size ) )
		{
			n_written += p7::write( document_destination, buffer, n_read );
			
			if ( dumping_progress )
			{
				dump_progress( n_written );
			}
		}
	}
	
	int Main( int argc, char** argv )
	{
		char *const *args = get_options( argv );
		
		const int argn = argc - (args - argv);
		
		if ( argn == 0 )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "htget: Usage:  htget <url>\n" ) );
			
			return 1;
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
			
			return 1;
		}
		
		plus::string  scheme;
		plus::string  hostname;
		plus::string  portStr;
		
		const char* default_port = "";
		
		const char* url_path = ParseURL( args[ 0 ], scheme, hostname, portStr );
		
		if ( url_path == NULL )
		{
			p7::perror( "htget", args[ 0 ], "Malformed URL" );
			
			return 2;
		}
		
		const size_t url_path_size = strlen( url_path );
		
		// FIXME:  Eliminate . and .. from url_path
		
		if ( saveToFile )
		{
			outputFile = DocName( url_path, url_path_size );
		}
		
		bool outputIsToFile = outputFile != defaultOutput;
		
		if ( scheme == "http" )
		{
			default_port = "80";
		}
		else
		{
			plus::string message = "Unsupported scheme '" + scheme + "'.\n";
			
			p7::write( p7::stderr_fileno, message );
			
			return 2;
		}
		
		const char* host = hostname.c_str();
		const char* port = default_port;
		
		if ( !portStr.empty() )
		{
			port = portStr.c_str();
		}
		
		plus::string message_header =   HTTP::RequestLine( method, url_path, url_path_size )
		                              + HTTP::HeaderFieldLine( "Host", hostname )
		                              + "User-Agent: " DEFAULT_USER_AGENT "\r\n"
		                              + "\r\n";
		
		n::owned< p7::fd_t > http_server = p7::connect( host, port );
		
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
			const plus::string& partial_content = response.GetPartialContent();
			
			if ( dumping_progress )
			{
				content_length = response.ContentLengthOrZero();
				
				if ( content_length )
				{
					content_length_string = gear::inscribe_unsigned_decimal( content_length );
					
					dump_progress( partial_content.size() );
				}
			}
			
			receive_document( partial_content,
			                  http_server,
			                  p7::open( outputFile, p7::o_wronly | create_flags ) );
		}
		
		return 0;
	}
	
}
