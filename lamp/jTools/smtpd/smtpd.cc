/*	========
 *	smtpd.cc
 *	========
 */

// Standard C++
#include <algorithm>
#include <functional>
#include <list>
#include <string>
#include <vector>

// Standard C/C++
#include <cstdlib>

// POSIX
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

// iota
#include "iota/decimal.hh"
#include "iota/strings.hh"

// plus
#include "plus/functional_extensions.hh"
#include "plus/pointer_to_function.hh"

// text-input
#include "text_input/feed.hh"
#include "text_input/get_line_from_feed.hh"

// Nucleus
#include "Nucleus/Shared.h"

// Io
#include "io/io.hh"
#include "io/spew.hh"

// Nitrogen
#include "Nitrogen/DateTimeUtils.h"
#include "Nitrogen/Folders.h"

// Io: MacFiles
#include "MacFiles/Classic.hh"

// poseven
#include "poseven/extras/fd_reader.hh"
#include "poseven/functions/write.hh"
#include "poseven/types/exit_t.hh"

// FSContents
#include "FSContents.h"

// Orion
#include "Orion/Main.hh"


namespace io
{
	
	inline FSSpec path_descent( const Nitrogen::FSDirSpec& dir, const unsigned char* name )
	{
		return Nitrogen::FSMakeFSSpec( dir, name );
	}
	
	inline FSSpec path_descent( const FSSpec& dir, const unsigned char* name )
	{
		return path_descent( Nucleus::Convert< Nitrogen::FSDirSpec >( dir ), name );
	}
	
}

namespace Nitrogen
{
	
	struct RecursiveFSDeleter
	{
		void operator()( const FSDirSpec& dir ) const
		{
			io::recursively_delete_directory( dir );
		}
	};
	
	class FSDirSpec_AlivenessTest
	{
		public:
			static bool IsLive( const FSDirSpec& dir )
			{
				return dir.dirID != 0;
			}
	};
	
}

namespace Nucleus
{
	
	template <>
	struct LivelinessTraits< Nitrogen::FSDirSpec, Nitrogen::RecursiveFSDeleter >
	{
		typedef Nitrogen::FSDirSpec_AlivenessTest LivelinessTest;
	};
	
}

namespace tool
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	using namespace io::path_descent_operators;
	
	
	// E.g. "19840124.183000"
	static std::string DateFormattedForFilename( unsigned long clock, int serial )
	{
		DateTimeRec date = N::SecondsToDate( clock );
		
		std::string result;
		
		result.resize( STRLEN( "YYYYMMDD.hhmmss-nn" ) );
		
		char* p = &result[0];
		
		iota::fill_unsigned_decimal( date.year,  p,     4 );
		iota::fill_unsigned_decimal( date.month, &p[4], 2 );
		iota::fill_unsigned_decimal( date.day,   &p[6], 2 );
		
		result[8] = '.';
		
		iota::fill_unsigned_decimal( date.hour,   &p[ 9], 2 );
		iota::fill_unsigned_decimal( date.minute, &p[11], 2 );
		iota::fill_unsigned_decimal( date.second, &p[13], 2 );
		
		result[15] = '-';
		
		iota::fill_unsigned_decimal( serial, &p[16], 2 );
		
		return result;
	}
	
	static std::string MakeMessageName()
	{
		static unsigned long stamp = N::GetDateTime();
		static int serial = 0;
		
		unsigned long now = N::GetDateTime();
		
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
	
	inline unsigned int IP( unsigned char a,
	                        unsigned char b,
	                        unsigned char c,
	                        unsigned char d )
	{
		return (a << 24) | (b << 16) | (c << 8) | d;
	}
	
	
	static std::string GetForwardPath( const std::string& rcptLine )
	{
		return rcptLine.substr( std::strlen( "RCPT TO:" ), rcptLine.npos );
	}
	
	static std::string GetReversePath( const std::string& fromLine )
	{
		return fromLine.substr( std::strlen( "MAIL FROM:" ), fromLine.npos );
	}
	
	static void CreateOneLiner( const FSSpec& file, const std::string& line )
	{
		typedef NN::StringFlattener< std::string > Flattener;
		
		std::string output = line + "\n";
		
		io::spew_file< Flattener >( N::FSpCreate( file,
		                                          N::OSType( 'R*ch' ),
		                                          N::OSType( 'TEXT' ) ),
		                            output );
	}
	
	static void CreateDestinationFile( const N::FSDirSpec& destFolder, const std::string& dest )
	{
		CreateOneLiner( destFolder / dest.substr( 0, 31 ),
		                dest );
	}
	
	static N::FSDirSpec QueueDirectory()
	{
		return NN::Convert< N::FSDirSpec >( io::system_root< N::FSDirSpec >() / "j" / "var" / "spool" / "jmail" / "queue" );
	}
	
	
	class PartialMessage
	{
		private:
			NN::Owned< N::FSDirSpec, N::RecursiveFSDeleter > dir;
			NN::Owned< N::FSFileRefNum > out;
			unsigned int bytes;
		
		public:
			PartialMessage()  {}
			PartialMessage( const FSSpec& dir );
			
			N::FSDirSpec Dir() const  { return dir; }
			unsigned int Bytes() const  { return bytes; }
			void WriteLine( const std::string& line );
			
			void Finished();
	};
	
	PartialMessage::PartialMessage( const FSSpec& dirLoc )
	:
		dir( NN::Owned< N::FSDirSpec, N::RecursiveFSDeleter >::Seize( N::FSpDirCreate( dirLoc ) ) ), 
		out( io::open_for_writing( N::FSpCreate( dir.Get() / "Message",
		                                         N::OSType( 'R*ch' ),
		                                         N::OSType( 'TEXT' ) ) ) )
	{
		//
	}
	
	void PartialMessage::WriteLine( const std::string& line )
	{
		//static unsigned int lastFlushKBytes = 0;
		std::string terminatedLine = line + "\r\n";
		
		io::write( out, terminatedLine.data(), terminatedLine.size() );
		
		bytes += terminatedLine.size();
		
		/*
		unsigned int kBytes = bytes / 1024;
		
		if ( kBytes - lastFlushKBytes >= 4 )
		{
			Io::Err << ".";
			//IO::Flush(out);
			lastFlushKBytes = kBytes;
		}
		*/
	}
	
	void PartialMessage::Finished()
	{
		dir.Release();
	}
	
	
	std::string myHello;
	std::string myFrom;
	std::list< std::string > myTo;
	PartialMessage myMessage;
	bool dataMode = false;
	
	
	static void QueueMessage()
	{
		N::FSDirSpec dir = myMessage.Dir();
		
		// Create the Destinations subdirectory.
		N::FSDirSpec destFolder = N::FSpDirCreate( dir / "Destinations" );
		
		// Create the destination files.
		std::for_each( myTo.begin(),
		               myTo.end(),
		               plus::compose1( std::bind1st( plus::ptr_fun( CreateDestinationFile ),
		                                             destFolder ),
		                               plus::ptr_fun( GetForwardPath ) ) );
		
		// Create the Return-Path file.
		// Write this last so the sender won't delete the message prematurely.
		CreateOneLiner( dir / "Return-Path", 
		                GetReversePath( myFrom ) );
		
	}
	
	static void DoCommand( const std::string& command )
	{
		std::string word = command.substr( 0, command.find(' ') );
		
		if ( false )
		{
			//
		}
		else if ( word == "MAIL" )
		{
			myFrom = command;
			
			p7::write( p7::stdout_fileno, STR_LEN( "250 Sender ok, probably"  "\r\n" ) );
		}
		else if ( word == "RCPT" )
		{
			myTo.push_back( command );
			
			p7::write( p7::stdout_fileno, STR_LEN( "250 Recipient ok, I guess"  "\r\n" ) );
		}
		else if ( word == "DATA" )
		{
			PartialMessage msg( QueueDirectory() / MakeMessageName() );
			
			myMessage = msg;
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
	
	static void DoData( const std::string& data )
	{
		myMessage.WriteLine( data );
		
		if ( data == "." )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "done\n" ) );
			
			dataMode = false;
			
			bool queued = false;
			
			try
			{
				QueueMessage();
				myMessage.Finished();
				queued = true;
			}
			catch ( ... )
			{
				
			}
			
			const char* message = queued ? "250 Message accepted"      "\r\n"
			                             : "554 Can't accept message"  "\r\n";
			
			p7::write( p7::stdout_fileno, message, std::strlen( message ) );
		}
		else
		{
			//
		}
	}
	
	static void DoLine( const std::string& line )
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
	
	int Main( int argc, iota::argv_t argv )
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
		
		char hostname[256] = "smtpd";
		
		char message_buffer[ buffer_size ] = "220 smtpd";
		
		(void) gethostname( message_buffer + STRLEN( "220 " ),
		                    max_hostname_length + 1 );
		
		const size_t partial_size = strlen( message_buffer );
		
		memcpy( message_buffer + partial_size, STR_LEN( " ready" "\r\n" ) + 1 );
		
		const size_t message_size = partial_size + STRLEN( " ready" "\r\n" );
		
		p7::write( p7::stdout_fileno, message_buffer, message_size );
		
		text_input::feed feed;
		
		p7::fd_reader reader( p7::stdin_fileno );
		
		while ( const std::string* s = get_line_from_feed( feed, reader ) )
		{
			std::string line( s->begin(), s->end() - 1 );
			
			DoLine( line );
		}
		
		return 0;
	}

}

