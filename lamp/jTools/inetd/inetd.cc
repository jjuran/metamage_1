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
#include <vfork.h>

// Iota
#include "iota/strings.hh"

// Nucleus
#include "Nucleus/NAssert.h"

// POSeven
#include "POSeven/Errno.hh"
#include "POSeven/Open.hh"

// Io
#include "Io/TextInput.hh"

// Orion
#include "Orion/Main.hh"


namespace NN = Nucleus;
namespace p7 = poseven;
namespace O = Orion;


struct Record
{
	short                       port;
	std::string                 path;
	std::vector< std::string >  argv;
};

static std::vector< Record > gRecords;

static std::map< int, Record > gServers;

static bool gChildSignalled = false;


static void FailedCall( const std::string& name, int error = errno )
{
	std::perror( ("inetd: " + name).c_str() );
	
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
		
		std::string message = "inetd: execv( ";
		
		message += argv[ 0 ];
		message += " ) failed";
		
		std::perror( message.c_str() );
		
		_exit( 1 );  // Use _exit() to exit a forked but not exec'ed process.
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
			std::perror( "inetd: select()" );
			
			return;
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
		p7::write( p7::stderr_fileno, STR_LEN( "Too few tokens in record\n" ) );
		
		O::ThrowExitStatus( 1 );
	}
	
	Record result;
	
	result.port = std::atoi( tokens[ kPort ].c_str() );
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
	Io::TextInputAdapter< NN::Owned< p7::fd_t > > input = io::open_for_reading( "/etc/inetd.conf" );
	
	while ( !input.Ended() )
	{
		std::string line = input.Read();
		
		ProcessLine( line );
	}
}

int O::Main( int /*argc*/, char const *const /*argv*/[] )
{
	if ( false )
	{
		p7::write( p7::stderr_fileno, STR_LEN( "Usage: daemon port command\n" ) );
		
		return 1;
	}
	
	p7::write( p7::stdout_fileno, STR_LEN( "Starting internet superserver: inetd" ) );
	
	(void)signal( 20, HandleSIGCHLD );
	
	ReadInetdDotConf();
	
	p7::write( p7::stdout_fileno, STR_LEN( ".\n" ) );
	
	if ( gServers.size() > 0 )
	{
		WaitForClients();
	}
	
	return 0;
}

