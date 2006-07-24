/*	========
 *	httpd.cc
 *	========
 */

// Standard C/c++
#include <cstdlib>
#include <cstring>

// Standard C++
#include <numeric>
#include <map>
#include <string>
#include <vector>

// POSIX
#include "sys/socket.h"
#include "sys/wait.h"
#include "unistd.h"

#include "vfork.h"

// Nitrogen
#include "Nitrogen/Folders.h"
#include "Nitrogen/OpenTransportProviders.h"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"

// Io
#include "Io/Exceptions.hh"
#include "Io/Files.hh"
#include "Io/MakeHandle.hh"

// BitsAndBytes
#include "HexStrings.hh"

// POSeven
#include "POSeven/Errno.hh"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"

// Kerosene
#include "SystemCalls.hh"


namespace N = Nitrogen;
namespace NN = Nucleus;
namespace P7 = POSeven;
namespace O = Orion;

using BitsAndBytes::EncodeAsHex;


static int ForkExecWait( const char* path, char const* const argv[] )
{
	int pid = vfork();
	
	if ( pid == -1 )
	{
		Io::Err << "vfork() failed\n";
		return -1;
	}
	
	if ( pid == 0 )
	{
		int exec_result = execv( path, const_cast< char* const* >( argv ) );
		
		if ( exec_result == -1 )
		{
			Io::Err << "execv( " << path << " ) failed\n";
			_exit( 1 );  // Use _exit() to exit a forked but not exec'ed process.
		}
	}
	
	int stat = -1;
	pid_t resultpid = waitpid( pid, &stat, 0 );
	
	if ( resultpid == -1 )
	{
		Io::Err << "waitpid() failed\n";
	}
	
	return stat;
}


struct HTTPRequestData
{
	std::vector< std::string > lines;
	
	std::string partialData;
	
	void Read();
};

class HTTPRequest
{
	private:
		HTTPRequestData data;
	
	public:
		HTTPRequest()  { data.lines.push_back( std::string() ); }
		HTTPRequest( const HTTPRequestData& data ) : data( data )  {}
		
		std::string Command() const  { return data.lines.front(); }
		std::string Method() const;
		std::string Resource() const;
		std::string HTTPVersion() const;
};

void HTTPRequestData::Read()
{
	try
	{
		while ( true )
		{
			char buf[ 1024 ];
			
			int received = Io::Read( Io::in, buf, 1024 );
			partialData += std::string( buf, received );
			
			// search for \r\n
			std::string::size_type cr = partialData.find( '\r' );
			
			while ( cr < partialData.npos  &&  partialData.size() >= cr + 2 )
			{
				if ( partialData[ cr + 1 ] != '\n' )
				{
					// CR not followed by LF -- bad input
					O::ThrowExitStatus( 1 );
				}
				
				if ( cr == 0 )
				{
					if ( lines.empty() )
					{
						// Double newline with no preceding request
						O::ThrowExitStatus( 1 );
					}
					
					//for_each( lines.begin() + 1, lines.end(), ParseHeader( myHeaders ) );
					
					//return lines.front();
					return;
				}
				else
				{
					std::string line = partialData.substr( 0, cr );
					Io::Err << line << "\n";
					lines.push_back( line );
					partialData = partialData.substr( cr + 2, partialData.npos );
					cr = partialData.find( '\r' );
				}
			}
		}
	}
	catch ( const P7::Errno& error )
	{
		Io::Err << "Read error: " << error.Get() << "\n";
	}
	catch ( const Io::EndOfInput& )
	{
		Io::Err << "Connection dropped by peer\n";
	}
	catch ( ... )
	{
		Io::Err << "Unrecognized exception caught\n";
	}
	
	O::ThrowExitStatus( 1 );
}

static std::string RequestType( const std::string& request )
{
	std::string::size_type end = request.find( ' ' );
	return request.substr( 0, end );
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

static FSSpec LocateResource( const std::string& resource )
{
	FSSpec docRoot = Path2FSS( "/var/www" );
	FSSpec item = docRoot;
	
	ResourceParser parser( resource );
	//FSSpec file = std::accumulate( parser.begin(), parser.end(), root );
	
	typedef ResourceParser::const_iterator const_iterator;
	for ( const_iterator it = parser.begin();  it != parser.end();  ++it )
	{
		N::FSDirSpec dir = NN::Convert< N::FSDirSpec >( item );
		item = dir & *it;
	}
	
	if ( N::FSpTestDirectoryExists( item ) )
	{
		item = NN::Convert< N::FSDirSpec >( item ) & "index.html";
	}
	
	if ( !N::FSpTestFileExists( item ) )
	{
		// item is missing or index.html is a directory
		throw N::FNFErr();
	}
	
	return item;
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

static std::string GuessContentType( const FSSpec& file )
{
	OSType type = N::FSpGetFInfo( file ).fdType;
	std::string ext = FilenameExtension( file.name );
	
	if ( ext == ".html" )
	{
		return "text/html";
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

static std::string HTTPHeader( const std::string& field, const std::string& value )
{
	return field + ": " + value + "\r\n";
}

static void DumpFile( const FSSpec& file )
{
	NN::Owned< N::FSFileRefNum > input( N::FSpOpenDF( file, fsRdPerm ) );
	
	int bytes;
	enum { dataSize = 512 };
	char data[ dataSize ];
	
	try
	{
		while ( true )
		{
			bytes = Io::Read( input, data, dataSize );
			Io::Put( Io::Out, data, bytes);
		}
	}
	catch ( const Io::EndOfInput& )
	{
		// end of file reached -- we're done
	}
}


static void SendResponse( const HTTPRequestData& request )
{
	ParsedRequest parsed = ParseRequest( request.lines.front() );
	
	std::string httpVersion = "HTTP/1.0";
	
	FSSpec file;
	
	try
	{
		file = LocateResource( parsed.resource );
	}
	catch ( const N::FNFErr& )
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
		
		std::string pathname = N::FSpGetPOSIXPathname( file );
		const char* path = pathname.c_str();
		
		char const* const argv[] = { path, NULL };
		
		int stat = ForkExecWait( path, argv );
	}
	else
	{
		FInfo info = N::FSpGetFInfo( file );
		
		std::string contentType = GuessContentType( file );
		
		std::string responseHeader = httpVersion + " 200 OK\r\n";
		
		responseHeader += HTTPHeader( "Content-Type",  contentType                   );
		responseHeader += HTTPHeader( "X-Mac-Type",    EncodeAsHex( info.fdType    ) );
		responseHeader += HTTPHeader( "X-Mac-Creator", EncodeAsHex( info.fdCreator ) );
		responseHeader += "\r\n";
		
		Io::Out << responseHeader;
		
		if ( parsed.method != "HEAD" )
		{
			DumpFile( file );
		}
	}
}

int O::Main( int argc, char const* const argv[] )
{
	InetAddress peer;
	socklen_t peerlen = sizeof peer;
	
	if ( getpeername( 0, (sockaddr*)&peer, &peerlen ) == 0 )
	{
		Io::Err << "Connection from "
		        << N::OTInetHostToString( peer.fHost )
		        << ", port "
		        << peer.fPort
		        << ".\n";
	}
	
	HTTPRequestData request;
	
	request.Read();
	
	SendResponse( request );
	
	return 0;
}

