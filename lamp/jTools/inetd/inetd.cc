/*	========
 *	inetd.cc
 *	========
 */

// Standard C++
#include <map>
#include <vector>

// POSIX
#include <sys/select.h>
#include <sys/wait.h>

// iota
#include "iota/strings.hh"

// gear
#include "gear/find.hh"
#include "gear/parse_decimal.hh"

// plus
#include "plus/string.hh"

// text-input
#include "text_input/feed.hh"
#include "text_input/get_line_from_feed.hh"

// poseven
#include "poseven/extras/fd_reader.hh"
#include "poseven/bundles/inet.hh"
#include "poseven/functions/accept.hh"
#include "poseven/functions/execv.hh"
#include "poseven/functions/fcntl.hh"
#include "poseven/functions/listen.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/perror.hh"
#include "poseven/functions/sigaction.hh"
#include "poseven/functions/socket.hh"
#include "poseven/functions/vfork.hh"
#include "poseven/functions/write.hh"
#include "poseven/types/exit_t.hh"

// Orion
#include "Orion/Main.hh"


namespace tool
{
	
	namespace n = nucleus;
	namespace p7 = poseven;
	
	
	struct Record
	{
		short                        port;
		plus::string                 path;
		std::vector< plus::string >  argv;
	};
	
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
				p7::perror( "inetd: select()" );
				
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
	
	
	static std::vector< plus::string > Split( const plus::string& text )
	{
		std::vector< plus::string > result;
		
		const unsigned char whitespace[] = { 2, ' ', '\t' };
		
		const char* begin = text.data();
		const char* end   = text.size() + begin;
		
		const char* word_start = gear::find_first_nonmatch( begin, end, whitespace );
		
		while ( const char* word_start = gear::find_first_nonmatch( begin, end, whitespace ) )
		{
			const char* word_end = gear::find_first_match( word_start, end, whitespace, end );
			
			result.push_back( plus::string( word_start, word_end ) );
			
			if ( word_end == end )
			{
				break;
			}
			
			begin = word_end;
		}
		
		return result;
	}
	
	static Record MakeRecord( const std::vector< plus::string >& tokens )
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
		
		result.port = gear::parse_unsigned_decimal( tokens[ kPort ].c_str() );
		result.path = tokens[ kPath ];
		
		std::size_t argc = tokens.size() - kArgv;
		
		result.argv.resize( argc );
		
		std::copy( tokens.begin() + kArgv, tokens.end(), result.argv.begin() );
		
		return result;
	}
	
	static void ProcessLine( const plus::string& line )
	{
		if ( gear::find_first_nonmatch( line.data(), line.size(), '\t', "#" )[0] == '#' )
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
		text_input::feed feed;
		
		n::owned< p7::fd_t > fd( p7::open( "/etc/inetd.conf", p7::o_rdonly ) );
		
		p7::fd_reader reader( fd );
		
		while ( const plus::string* s = get_line_bare_from_feed( feed, reader ) )
		{
			plus::string line( *s );
			
			ProcessLine( line );
		}
	}
	
	int Main( int argc, char** argv )
	{
		if ( false )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "Usage: daemon port command\n" ) );
			
			return 1;
		}
		
		p7::write( p7::stdout_fileno, STR_LEN( "Starting internet superserver: inetd" ) );
		
		p7::sigaction( p7::sigchld, HandleSIGCHLD );
		
		ReadInetdDotConf();
		
		p7::write( p7::stdout_fileno, STR_LEN( ".\n" ) );
		
		if ( gServers.size() > 0 )
		{
			WaitForClients();
		}
		
		return 0;
	}
	
}
