/*	========
 *	smtpd.cc
 *	========
 */

// Standard C++
#include <memory>
#include <vector>

// Standard C/C++
#include <cstdio>
#include <cstdlib>

// Standard C
#include <time.h>

// POSIX
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

// more-libc
#include "more/string.h"

// iota
#include "iota/strings.hh"

// gear
#include "gear/inscribe_decimal.hh"

// plus
#include "plus/string/concat.hh"

// text-input
#include "text_input/feed.hh"
#include "text_input/get_line_from_feed.hh"

// poseven
#include "poseven/extras/fd_reader.hh"
#include "poseven/extras/spew.hh"
#include "poseven/functions/mkdir.hh"
#include "poseven/functions/write.hh"
#include "poseven/types/exit_t.hh"

// pfiles
#include "pfiles/common.hh"

// Io
#include "io/walk.hh"

// Orion
#include "Orion/Main.hh"

#ifdef __APPLE__
#ifndef _SOCKLEN_T
typedef int socklen_t;
#endif
#endif


namespace tool
{
	
	namespace n = nucleus;
	namespace p7 = poseven;
	
	using namespace io::path_descent_operators;
	
	
	// E.g. "19840124.183000"
	static plus::string DateFormattedForFilename( const time_t& now, int serial )
	{
		const struct tm* t = gmtime( &now );
		
		plus::string result;
		
		char* p = result.reset( STRLEN( "YYYYMMDD.hhmmss-nn" ) );
		
		gear::fill_unsigned_decimal( t->tm_year + 1900, &p[0], 4 );
		gear::fill_unsigned_decimal( t->tm_mon  +    1, &p[4], 2 );
		gear::fill_unsigned_decimal( t->tm_mday,        &p[6], 2 );
		
		p[8] = '.';
		
		gear::fill_unsigned_decimal( t->tm_hour, &p[ 9], 2 );
		gear::fill_unsigned_decimal( t->tm_min,  &p[11], 2 );
		gear::fill_unsigned_decimal( t->tm_sec,  &p[13], 2 );
		
		p[15] = '-';
		
		gear::fill_unsigned_decimal( serial, &p[16], 2 );
		
		return result;
	}
	
	static plus::string MakeMessageName()
	{
		static time_t stamp = 0;
		static int serial = 0;
		
		const time_t now = time( NULL );
		
		if ( stamp == now )
		{
			++serial;
		}
		else
		{
			stamp = now;
			serial = 1;
		}
		
		return DateFormattedForFilename( now, serial );
	}
	
	static plus::string GetForwardPath( const plus::string& rcptLine )
	{
		return rcptLine.substr( STRLEN( "RCPT TO:" ) );
	}
	
	static plus::string GetReversePath( const plus::string& fromLine )
	{
		return fromLine.substr( STRLEN( "MAIL FROM:" ) );
	}
	
	static void CreateOneLiner( const plus::string& path, const plus::string& line )
	{
		plus::string output = line + "\n";
		
		p7::spew( p7::open( path, p7::o_wronly | p7::o_creat | p7::o_excl ), output );
	}
	
	static void CreateDestinationFile( const plus::string& dest_dir_path, const plus::string& dest )
	{
		CreateOneLiner( dest_dir_path / dest,
		                dest );
	}
	
	static const char* QueueDirectory()
	{
		return "/var/spool/mail/queue";
	}
	
	
	class PartialMessage
	{
		private:
			plus::string          dir;
			n::owned< p7::fd_t >  out;
		
		private:
			// non-copyable
			PartialMessage           ( const PartialMessage& );
			PartialMessage& operator=( const PartialMessage& );
		
		public:
			PartialMessage( const plus::string& dir );
			
			~PartialMessage();
			
			const plus::string& Dir() const  { return dir; }
			void WriteLine( const plus::string& line );
			
			void Finished();
	};
	
	PartialMessage::PartialMessage( const plus::string& dirLoc )
	:
		dir( dirLoc )
	{
		p7::mkdir( dir );
		
		out = p7::open( dir / "Message", p7::o_wronly | p7::o_creat | p7::o_excl, p7::_400 );
	}
	
	PartialMessage::~PartialMessage()
	{
		if ( !dir.empty() )
		{
			io::recursively_delete_directory( dir );
		}
	}
	
	void PartialMessage::WriteLine( const plus::string& line )
	{
		//static unsigned int lastFlushKBytes = 0;
		plus::string terminatedLine = line + "\r\n";
		
		p7::write( out, terminatedLine );
	}
	
	void PartialMessage::Finished()
	{
		dir.reset();
	}
	
	
	plus::string myHello;
	plus::string myFrom;
	std::vector< plus::string > myTo;
	
	static std::auto_ptr< PartialMessage > myMessage;
	
	bool dataMode = false;
	
	
	static void QueueMessage()
	{
		const plus::string& dir = myMessage->Dir();
		
		// Create the Destinations subdirectory.
		plus::string destinations_dir = dir / "Destinations";
		
		p7::mkdir( destinations_dir );
		
		// Create the destination files.
		
		for ( size_t i = 0;  i < myTo.size();  ++i )
		{
			const plus::string& line = myTo[ i ];
			
			CreateOneLiner( destinations_dir / line, line );
		}
		
		// Create the Return-Path file.
		// Write this last so the sender won't delete the message prematurely.
		CreateOneLiner( dir / "Return-Path", 
		                myFrom );
		
	}
	
	static void DoCommand( const plus::string& command )
	{
		plus::string word = command.substr( 0, command.find(' ') );
		
		if ( false )
		{
			//
		}
		else if ( word == "MAIL" )
		{
			myFrom = GetReversePath( command );
			
			p7::write( p7::stdout_fileno, STR_LEN( "250 Sender ok, probably"  "\r\n" ) );
		}
		else if ( word == "RCPT" )
		{
			myTo.push_back( GetForwardPath( command ) );
			
			p7::write( p7::stdout_fileno, STR_LEN( "250 Recipient ok, I guess"  "\r\n" ) );
		}
		else if ( word == "DATA" )
		{
			myMessage.reset( new PartialMessage( QueueDirectory() / MakeMessageName() ) );
			dataMode  = true;
			
			p7::write( p7::stdout_fileno, STR_LEN( "354 I'm listening"  "\r\n" ) );
		}
		else if ( word == "HELO" )
		{
			myHello = command;
			
			p7::write( p7::stdout_fileno, STR_LEN( "250 Hello there"  "\r\n" ) );
		}
		else if ( word == "RSET" )
		{
			myFrom = "";
			myTo.clear();
			//myData.clear();
			
			p7::write( p7::stdout_fileno, STR_LEN( "250 Reset"  "\r\n" ) );
		}
		else if ( word == "QUIT" )
		{
			//isComplete = true;
			p7::write( p7::stdout_fileno, STR_LEN( "221 Closing connection"  "\r\n" ) );
			
			throw p7::exit_success;
		}
		else
		{
			if ( word != "EHLO" )
			{
				std::fprintf( stderr, "Unrecognized command '%s'\n", word.c_str() );
			}
			
			p7::write( p7::stdout_fileno, STR_LEN( "500 Unrecognized command"  "\r\n" ) );
		}
	}
	
	static void DoData( const plus::string& data )
	{
		myMessage->WriteLine( data );
		
		if ( data == "." )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "done\n" ) );
			
			dataMode = false;
			
			bool queued = false;
			
			try
			{
				QueueMessage();
				myMessage->Finished();
				queued = true;
			}
			catch ( ... )
			{
				
			}
			
			const char* message = queued ? "250 Message accepted"      "\r\n"
			                             : "554 Can't accept message"  "\r\n";
			
			p7::write( p7::stdout_fileno, message, std::strlen( message ) );
			
			myMessage.reset();
		}
		else
		{
			//
		}
	}
	
	static void DoLine( const plus::string& line )
	{
		if ( dataMode )
		{
			DoData( line );
		}
		else
		{
			DoCommand( line );
		}
	}
	
	int Main( int argc, char** argv )
	{
		sockaddr_in peer;
		socklen_t peerlen = sizeof peer;
		
		if ( getpeername( 0, (sockaddr*)&peer, &peerlen ) == 0 )
		{
			std::fprintf( stderr, "Connection from %s, port %d\n",
			                                       inet_ntoa( peer.sin_addr ),
			                                                peer.sin_port );
		}
		
		const size_t max_hostname_length = 255;
		
		const size_t buffer_size = STRLEN( "220 " )
		                         + max_hostname_length
		                         + STRLEN( " ready" "\r\n" )
		                         + 1;
		
		char message_buffer[ buffer_size ] = "220 smtpd";
		
		(void) gethostname( message_buffer + STRLEN( "220 " ),
		                    max_hostname_length + 1 );
		
		const size_t partial_size = strlen( message_buffer );
		
		mempcpy( message_buffer + partial_size, STR_LEN( " ready" "\r\n" ) + 1 );
		
		const size_t message_size = partial_size + STRLEN( " ready" "\r\n" );
		
		p7::write( p7::stdout_fileno, message_buffer, message_size );
		
		text_input::feed feed;
		
		p7::fd_reader reader( p7::stdin_fileno );
		
		while ( const plus::string* s = get_line_bare_from_feed( feed, reader ) )
		{
			plus::string line( *s );
			
			DoLine( line );
		}
		
		return 0;
	}

}
