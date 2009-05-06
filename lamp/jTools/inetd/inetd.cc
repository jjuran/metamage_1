/*	========
 *	inetd.cc
 *	========
 */

// Standard C++
#include <map>
#include <string>
#include <vector>

// Standard C/C++
#include <cstring>

// Standard C
#include <errno.h>

// POSIX
#include <fcntl.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/wait.h>

// Iota
#include "iota/strings.hh"

// Nucleus
#include "Nucleus/NAssert.h"

// POSeven
#include "POSeven/FileDescriptor.hh"
#include "POSeven/Open.hh"
#include "POSeven/bundles/inet.hh"
#include "POSeven/functions/accept.hh"
#include "POSeven/functions/execv.hh"
#include "POSeven/functions/fcntl.hh"
#include "POSeven/functions/listen.hh"
#include "POSeven/functions/signal.hh"
#include "POSeven/functions/socket.hh"
#include "POSeven/functions/vfork.hh"
#include "POSeven/functions/write.hh"
#include "POSeven/types/exit_t.hh"

// Io
#include "Io/TextInput.hh"

// Orion
#include "Orion/Main.hh"


namespace tool
{
	
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	
	struct Record
	{
		short                       port;
		std::string                 path;
		std::vector< std::string >  argv;
	};
	
	static std::vector< Record > gRecords;
	
	static std::map< int, Record > gServers;
	
	static bool gChildSignalled = false;
	
	
	static void HandleSIGCHLD( int )
	{
		gChildSignalled = true;
	}
	
	static void ServiceClient( p7::fd_t client, const char *const argv[] )
	{
		p7::pid_t pid = POSEVEN_VFORK();
		
		if ( pid == 0 )
		{
			dup2( client, 0 );
			dup2( client, 1 );
			//dup2( client, 2 );  // Keep error log.  login will dup2 1 -> 2.
			
			int result = close( client );
			
			p7::execv( argv );
		}
	}
	
	static void WaitForClients()
	{
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
			
			// This blocks and yields to other threads
			int selected = select( maxFD + 1, &readfds, NULL, NULL, NULL );
			
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
					
					const char* path = gServers[ listener ].path.c_str();
					
					const char* const argv[] = { path, NULL };
					
					// This won't block unless select() is broken
					// FIXME:  Yes it will, due to a race condition
					ServiceClient( p7::accept( p7::fd_t( listener ) ), argv );
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
			
			throw p7::exit_failure;
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
		
		Record record = MakeRecord( Split( line ) );
		
		p7::in_port_t port = p7::in_port_t( record.port );
		
	#ifdef SOCK_CLOEXEC
		
		const p7::socket_type type = p7::sock_stream | p7::sock_cloexec;
		
	#else
		
		const p7::socket_type type = p7::sock_stream;
		
	#endif
		
		const p7::fd_t listener = p7::bind( p7::inaddr_any, port, type ).release();
		
	#ifndef SOCK_CLOEXEC
		
		p7::fcntl< p7::f_setfd >( listener, p7::fd_cloexec );
		
	#endif
		
		gServers[ listener ] = record;
		
		p7::listen( listener, 3 );
	}
	
	static void ReadInetdDotConf()
	{
		Io::TextInputAdapter< NN::Owned< p7::fd_t > > input( io::open_for_reading( "/etc/inetd.conf" ) );
		
		while ( !input.Ended() )
		{
			std::string line = input.Read();
			
			ProcessLine( line );
		}
	}
	
	int Main( int argc, iota::argv_t argv )
	{
		if ( false )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "Usage: daemon port command\n" ) );
			
			return 1;
		}
		
		p7::write( p7::stdout_fileno, STR_LEN( "Starting internet superserver: inetd" ) );
		
		p7::signal( p7::sigchld, HandleSIGCHLD );
		
		ReadInetdDotConf();
		
		p7::write( p7::stdout_fileno, STR_LEN( ".\n" ) );
		
		if ( gServers.size() > 0 )
		{
			WaitForClients();
		}
		
		return 0;
	}
	
}

