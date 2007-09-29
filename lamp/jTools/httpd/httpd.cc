/*	========
 *	httpd.cc
 *	========
 */

// Standard C++
#include <numeric>
#include <map>
#include <string>
#include <vector>

// Standard C/C++
#include <cctype>
#include <cstdlib>
#include <cstring>

// POSIX
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

// Nucleus
#include "Nucleus/NAssert.h"

// POSeven
#include "POSeven/Directory.hh"
#include "POSeven/Errno.hh"
#include "POSeven/Open.hh"
#include "POSeven/Pathnames.hh"
#include "POSeven/Stat.hh"

// Nitrogen
#include "Nitrogen/Files.h"

// BitsAndBytes
#include "HexStrings.hh"

// Arcana
#include "HTTP.hh"

// Orion
#include "Orion/GetOptions.hh"
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"

// Kerosene
#if TARGET_OS_MAC && !TARGET_RT_MAC_MACHO
#include "SystemCalls.hh"
#endif

namespace N = Nitrogen;
namespace NN = Nucleus;
namespace P7 = POSeven;
namespace p7 = poseven;
namespace O = Orion;

using namespace io::path_descent_operators;

using BitsAndBytes::EncodeAsHex;


static const char* gDocumentRoot = "/var/www";


char ToCGI( char c )
{
	return c == '-' ? '_' : std::toupper( c );
}

static void SetCGIVariables( const HTTP::MessageReceiver& request )
{
	const HTTP::HeaderIndex& index = request.GetHeaderIndex();
	
	const char* stream = request.GetHeaderStream();
	
	for ( HTTP::HeaderIndex::const_iterator it = index.begin();  it != index.end();  ++it )
	{
		std::string header( stream + it->header_offset,
		                    stream + it->colon_offset );
		
		std::string value( stream + it->value_offset,
		                   stream + it->crlf_offset );
		
		std::transform( header.begin(),
		                header.end(),
		                header.begin(),
		                std::ptr_fun( ToCGI ) );
		
		if ( header == "CONTENT_TYPE"  ||  header == "CONTENT_LENGTH" )
		{
		}
		else
		{
			header = "HTTP_" + header;
		}
		
		setenv( header.c_str(), value.c_str(), 1 );
	}
}

static int ForkExecWait( const char*                   path,
                         char const* const             argv[],
                         const HTTP::MessageReceiver&  request )
{
	const std::string& partialData = request.GetPartialContent();
	
	bool partial_data_exist = !partialData.empty();
	
	int pipe_ends[2];
	
	if ( partial_data_exist )
	{
		pipe( pipe_ends );
	}
	
	int reader = pipe_ends[0];
	int writer = pipe_ends[1];
	
	int pid = vfork();
	
	if ( pid == -1 )
	{
		Io::Err << "vfork() failed\n";
		return -1;
	}
	
	if ( pid == 0 )
	{
		if ( partial_data_exist )
		{
			close( writer );
			
			dup2( reader, p7::stdin_fileno );  // read from pipe instead of socket
			
			close( reader );
		}
		
		// This eliminates LOCAL_EDITOR, PATH, and SECURITYSESSIONID.
		// We'll have to consider other approaches.
		//clearenv();
		
		SetCGIVariables( request );
		
		int exec_result = execv( path, const_cast< char* const* >( argv ) );
		
		if ( exec_result == -1 )
		{
			Io::Err << "execv( " << path << " ) failed\n";
			_exit( 1 );  // Use _exit() to exit a forked but not exec'ed process.
		}
	}
	
	if ( partial_data_exist )
	{
		close( reader );
		
		write( writer, partialData.data(), partialData.size() );
		
		enum { size = 4096 };
		
		char data[ size ];
		
		int bytes;
		
		while ( (bytes = read( p7::stdin_fileno, data, size )) > 0 )
		{
			write( writer, data, bytes );
		}
		
		if ( bytes == -1 )
		{
			std::perror( "read" );
		}
		
		close( writer );
	}
	
	int stat = -1;
	pid_t resultpid = waitpid( pid, &stat, 0 );
	
	if ( resultpid == -1 )
	{
		Io::Err << "waitpid() failed\n";
	}
	
	return stat;
}


struct ParsedRequest
{
	std::string method;
	std::string resource;
	std::string version;
};

static ParsedRequest ParseRequest( const std::string& request )
{
	// E.g.  "GET / HTTP/1.0"
	
	ParsedRequest parsed;
	
	// Find the first space (which ends the request method)
	std::string::size_type end = request.find( ' ' );
	
	parsed.method = request.substr( 0, end - 0 );  // e.g. "GET"
	
	// The resource starts after the space
	std::string::size_type resource = end + 1;
	
	// and ends with the next space
	end = request.find( ' ', resource );
	
	parsed.resource = request.substr( resource, end - resource );  // e.g. "/logo.png"
	
	// HTTP version string starts after the second space
	std::string::size_type version = end + 1;
	// and runs to the end
	end = std::string::npos;
	
	parsed.version = request.substr( version, end - version );  // e.g. "HTTP/1.1"
	
	return parsed;
}

class ResourceParser
{
	private:
		std::string resource;
	
	public:
		class const_iterator
		{
			private:
				const std::string& s;
				std::string::size_type pos, end;
			
			public:
				enum { npos = std::string::npos };
				struct End  {};
				static End kEnd;
				
				const_iterator( const std::string& s )
				:
					s  ( s                  ),
					pos( 1                  ),
					end( s.find( '/', pos ) )
				{}
				
				const_iterator( const std::string& s, End )
				:
					s  ( s    ),
					pos( npos ),
					end( npos )
				{}
				
				std::string operator*() const  { return s.substr( pos, end - pos ); }
				
				const_iterator& operator++()
				{
					if ( end + 1 < s.size() )
					{
						pos = end + 1;
						end = s.find( '/' , pos );
					}
					else
					{
						pos = end = std::string::npos;
					}
					
					return *this;
				}
				
				const_iterator operator++( int )
				{
					const_iterator old = *this;
					++*this;
					return old;
				}
				
				friend bool operator==( const const_iterator& a, const const_iterator& b )
				{
					return a.pos == b.pos
						&& a.end == b.end;
				}
				
				friend bool operator!=( const const_iterator& a, const const_iterator& b )
				{
					return !( a == b );
				}
		};
		
		ResourceParser( const std::string& resource ) : resource( resource + "/" )  {}
		
		const_iterator begin() const  { return const_iterator( resource                       ); }
		const_iterator end()   const  { return const_iterator( resource, const_iterator::kEnd ); }
};

static std::string LocateResource( const std::string& resource )
{
	ResourceParser parser( resource );
	
	// FIXME:  This can be an algorithm
	typedef ResourceParser::const_iterator const_iterator;
	for ( const_iterator it = parser.begin();  it != parser.end();  ++it )
	{
		std::string component = *it;
		
		if ( !component.empty() && component[0] == '.' )
		{
			// No screwing around with the pathname, please.
			P7::ThrowErrno( ENOENT );
		}
	}
	
	std::string pathname = gDocumentRoot + resource;
	
	p7::stat( pathname );  // Throw if nonexistent
	
	return pathname;
}

static std::string FilenameExtension(const std::string& filename)
{
	// This will break for index.html.en and similar
	std::string::size_type lastDot = filename.find_last_of( "." );
	
	if ( lastDot != std::string::npos )
	{
		return filename.substr( lastDot, std::string::npos );
	}
	else
	{
		return "";
	}
}

static std::string FilenameExtension(const unsigned char* filename)
{
	return FilenameExtension( NN::Convert< std::string >( filename ) );
}

static std::string GuessContentType( const std::string& filename, ::OSType type )
{
	std::string ext = FilenameExtension( filename );
	
	if ( ext == ".html" )
	{
		return "text/html";
	}
	else if ( ext == ".css" )
	{
		return "text/css";
	}
	else if ( type == 'TEXT' )
	{
		return "text/plain";
	}
	else if ( type == 'BINA' )
	{
		return "application/x-macbinary";
	}
	
	return "application/octet-stream";
}

static void DumpFile( const std::string& pathname )
{
	HTTP::SendMessageBody( p7::stdout_fileno, io::open_for_reading( pathname ) );
}

static void ListDir( const std::string& pathname )
{
	typedef io::directory_contents_traits< std::string >::container_type directory_container;
	
	directory_container contents = io::directory_contents( pathname );
	
	typedef directory_container::const_iterator Iter;
	
	for ( Iter it = contents.begin();  it != contents.end();  ++it )
	{
		std::string listing = io::get_filename_string( *it );
		
		listing += "\n";
		
		io::write( p7::stdout_fileno, listing.data(), listing.size() );
	}
	
}

static void SendError( const std::string& error )
{
	std::string httpVersion = "HTTP/1.0";
	
	std::string message = httpVersion + " " + error +     "\r\n"
	                      "Content-Type: text/html"       "\r\n"
	                                                      "\r\n"
	                      "<title>" + error + "</title>"  "\r\n"
	                      "<p>"     + error + "</p>"      "\r\n";
	
		io::write( p7::stdout_fileno, message.data(), message.size() );
}

static void SendResponse( const HTTP::MessageReceiver& request )
{
	ParsedRequest parsed = ParseRequest( request.GetStatusLine() );
	
	std::string httpVersion = "HTTP/1.0";
	
	std::string pathname;
	
	try
	{
		pathname = LocateResource( parsed.resource );
	}
	catch ( ... )
	{
		Io::Out << httpVersion + " 404 Not Found"                "\r\n"
		                         "Content-Type: text/html"       "\r\n"
		                                                         "\r\n"
		                         "<title>404 Not Found</title>"  "\r\n"
		                         "<p>404 Not Found</p>"          "\r\n";
		
		return;
	}
	
	std::string cgiBin = "/cgi-bin/";
	
	if ( parsed.resource.substr( 0, cgiBin.size() ) == cgiBin )
	{
		//Io::Out << httpVersion + " 200 OK\r\n";
		
		const char* path = pathname.c_str();
		
		char const* const argv[] = { path, NULL };
		
		int stat = ForkExecWait( path, argv, request );
	}
	else
	{
		bool is_dir = false;
		
		OSType type = kUnknownType;
		
		if ( io::directory_exists( pathname ) )
		{
			if ( *pathname.rbegin() != '/' )
			{
				SendError( "404 Not Found" );
				
				return;
			}
			
			std::string index_html = pathname / "index.html";
			
			if ( io::file_exists( index_html ) )
			{
				pathname = index_html;
			}
			else
			{
				is_dir = true;
			}
		}
		
		FInfo info = { 0 };
		
	#if TARGET_OS_MAC && !TARGET_RT_MAC_MACHO
		
		if ( !is_dir )
		{
			FSSpec file = Path2FSS( pathname );
			
			info = N::FSpGetFInfo( file );
			
			type = info.fdType;
		}
		
	#endif
		
		std::string contentType = is_dir ? "text/plain" : GuessContentType( pathname, type );
		
		std::string responseHeader = httpVersion + " 200 OK\r\n";
		
		responseHeader += HTTP::HeaderLine( "Content-Type",  contentType                   );
		
	#if TARGET_OS_MAC && !TARGET_RT_MAC_MACHO
		
		responseHeader += HTTP::HeaderLine( "X-Mac-Type",    EncodeAsHex( info.fdType    ) );
		responseHeader += HTTP::HeaderLine( "X-Mac-Creator", EncodeAsHex( info.fdCreator ) );
		
	#endif
		
		responseHeader += "\r\n";
		
		Io::Out << responseHeader;
		
		if ( parsed.method != "HEAD" )
		{
			is_dir ? ListDir( pathname ) : DumpFile( pathname );
		}
	}
}

int O::Main( int argc, argv_t argv )
{
	O:BindOption( "--doc-root", gDocumentRoot );
	
	O::GetOptions( argc, argv );
	
	sockaddr_in peer;
	socklen_t peerlen = sizeof peer;
	
	if ( getpeername( 0, (sockaddr*)&peer, &peerlen ) == 0 )
	{
		Io::Err << "Connection from "
		        << inet_ntoa( peer.sin_addr )
		        << ", port "
		        << peer.sin_port
		        << ".\n";
	}
	
	HTTP::MessageReceiver request;
	
	request.ReceiveHeaders( p7::stdin_fileno );
	
	SendResponse( request );
	
	return 0;
}

