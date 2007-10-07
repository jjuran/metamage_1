/*	========
 *	inetd.cc
 *	========
 */

// Standard C++
#include <map>

// Standard C/C++
#include <cstring>

// Standard C
#include <errno.h>

// POSIX
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

// Nucleus
#include "Nucleus/NAssert.h"

// Nitrogen
#include "Nitrogen/OpenTransportProviders.h"

// POSeven
#include "POSeven/Errno.hh"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"


namespace N = Nitrogen;
namespace p7 = poseven;
namespace O = Orion;


struct Record
{
	N::InetPort port;
	std::string path;
	std::vector< std::string > argv;
};

static std::vector< Record > gRecords;

static std::map< int, Record > gServers;

static bool gChildSignalled = false;


static void FailedCall( const std::string& name, int error = errno )
{
	Io::Err << name << "() failed:  " << error << "\n";
	O::ThrowExitStatus( 1 );
}

static void HandleSIGCHLD( int )
{
	gChildSignalled = true;
}

static int ForkCommand( int client, const char* path, char *const argv[] )
{
	int pid = vfork();
	
	if ( pid == 0 )
	{
		dup2( client, 0 );
		dup2( client, 1 );
		//dup2( client, 2 );  // Keep error log.  login will dup2 1 -> 2.
		
		int result = close( client );
		
		result = execv( path, argv );
		
		if ( result == -1 )
		{
			Io::Err << "execv( " << path << " ) failed\n";
			_exit( 1 );  // Use _exit() to exit a forked but not exec'ed process.
		}
	}
	
	return pid;
}

static void ServiceClient( int client, const char* path, char *const argv[] )
{
	int pid = ForkCommand( client, path, argv );
	
	int result = close( client );
}

static void WaitForClients()
{
	struct sockaddr_in from;
	socklen_t len = sizeof from;
	
	int maxFD = -1;
	
	fd_set listeners;
	
	FD_ZERO( &listeners );
	
	typedef std::map< int, Record >::const_iterator const_iterator;
	
	for ( const_iterator it = gServers.begin();  it != gServers.end();  ++it )
	{
		FD_SET( it->first, &listeners );
		
		maxFD = std::max( maxFD, it->first );
	}
	
	while ( true )
	{
		fd_set readfds = listeners;
		
		struct timeval timeout = { 5, 0 };
		
		// This blocks and yields to other threads
		int selected = select( maxFD + 1, &readfds, NULL, NULL, &timeout );
		
		if ( selected == -1 && errno != EINTR )
		{
			int error = errno;
			Io::Err << "select() failed:  " << error << "\n";
			return;
		}
		else
		{
			//Io::Err << "select() returned " << selected << "\n";
		}
		
		if ( gChildSignalled )
		{
			gChildSignalled = false;
			
			int stat;
			while ( waitpid( -1, &stat, 1 ) > 0 ) continue;
		}
		
		if ( selected > 0 )
		{
			for ( int listener = 0;  listener <= maxFD;  ++listener )
			{
				if ( !FD_ISSET( listener, &readfds ) )  continue;
				
				// This won't block unless select() is broken
				int client = accept( listener, (sockaddr*)&from, &len );
				
				if ( client == -1 )
				{
					std::perror( "inetd: accept() failed" );
					
					return;
				}
				
				const char* path = gServers[ listener ].path.c_str();
				char* const argv[] = { (char*)path, (char*)NULL };
				
				ServiceClient( client, path, argv );
			}
		}
	}
}

static std::string ReadLine( int fd )
{
	ByteCount minBytes = 1;
	
	const char* nl = NULL;
	std::size_t size = 0;
	
	do
	{
		const char* p;
		int peeked = peek( fd, &p, minBytes );
		
		p7::throw_posix_result( peeked );
		
		if ( peeked == 0 )
		{
			throw io::end_of_input();
		}
		
		const char* cr = std::strchr( p, '\r' );
		const char* lf = std::strchr( p, '\n' );
		
		nl = std::min( cr, lf );
		
		if ( nl == NULL )
		{
			// One or both chars are absent
			nl = std::max( cr, lf );
		}
		
		if ( nl == NULL )
		{
			// Both chars are absent
			minBytes = std::strlen( p ) + 1;
		}
		else
		{
			// nl is the first newline char
			size = nl - p;
		}
	}
	while ( nl == NULL );
	
	std::string result;
	
	result.resize( size + 1 );
	
	int bytes = read( fd, const_cast< char* >( result.data() ), size + 1 );
	
	ASSERT( bytes == size + 1 );
	
	result.resize( size );
	
	return result;
}

static std::vector< std::string > Split( const std::string& text )
{
	std::vector< std::string > result;
	
	std::size_t word = text.find_first_not_of( " \t" );
	
	while ( word != text.npos )
	{
		std::size_t ws = text.find_first_of( " \t", word );
		
		result.push_back( text.substr( word, ws - word ) );
		
		if ( ws == text.npos )
		{
			break;
		}
		
		word = text.find_first_not_of( " \t", ws );
	}
	
	return result;
}

static Record MakeRecord( const std::vector< std::string >& tokens )
{
	enum
	{
		kPort     = 0,
		kType     = 1,
		kProtocol = 2,
		kWait     = 3,
		kUser     = 4,
		kPath     = 5,
		kArgv     = 6
	};
	
	if ( tokens.size() < 7 )
	{
		Io::Err << "Too few tokens in record\n";
		O::ThrowExitStatus( 1 );
	}
	
	Record result;
	
	result.port = N::InetPort( std::atoi( tokens[ kPort ].c_str() ) );
	result.path = tokens[ kPath ];
	
	std::size_t argc = tokens.size() - kArgv;
	
	result.argv.resize( argc );
	
	std::copy( tokens.begin() + kArgv, tokens.end(), result.argv.begin() );
	
	return result;
}

static void ProcessLine( const std::string& line )
{
	std::size_t first = line.find_first_not_of( " \t" );
	
	if ( first == line.npos || line[ first ] == '#' )
	{
		return;  // It's blank or a comment
	}
	
	int listener = socket( PF_INET, SOCK_STREAM, INET_TCP );
	
	if ( listener == -1 )
	{
		FailedCall( "socket" );
	}
	
	gServers[ listener ] = MakeRecord( Split( line ) );
	
	struct sockaddr_in inetAddress;
	
	inetAddress.sin_family = AF_INET;
	inetAddress.sin_port = gServers[ listener ].port;
	inetAddress.sin_addr.s_addr = kOTAnyInetAddress;
	
	int result = bind( listener, (const sockaddr*)&inetAddress, sizeof (sockaddr_in) );
	
	if ( result == -1 )
	{
		FailedCall( "bind" );
	}
	
	result = listen( listener, 3 );
	
	if ( result == -1 )
	{
		FailedCall( "listen" );
	}
}

static void ReadInetdDotConf()
{
	int fd = open( "/etc/inetd.conf", O_RDONLY );
	
	if ( fd == -1 )  return;
	
	try
	{
		while ( true )
		{
			ProcessLine( ReadLine( fd ) );
		}
	}
	catch ( const io::end_of_input& ) {}
	
	close( fd );
}

int O::Main( int /*argc*/, char const *const /*argv*/[] )
{
	if ( false )
	{
		Io::Err << "Usage: daemon port command\n";
		return 1;
	}
	
	Io::Out << "Starting internet superserver: inetd";
	
	(void)signal( 20, HandleSIGCHLD );
	
	ReadInetdDotConf();
	
	Io::Out << ".\n";
	
	if ( gServers.size() > 0 )
	{
		WaitForClients();
	}
	
	return 0;
}

