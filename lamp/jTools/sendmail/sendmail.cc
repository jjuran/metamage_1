/*	===========
 *	sendmail.cc
 *	===========
 */

// Standard C++
#include <algorithm>
#include <string>
#include <vector>

// Standard C/C++
#include <cctype>

// Mac OS
#ifndef __OPENTRANSPORT__
#include <OpenTransport.h>
#endif

// POSIX
#include <arpa/inet.h>
#include <netdb.h>

// Iota
#include "iota/decimal.hh"

// plus
#include "plus/pointer_to_function.hh"

// Nitrogen
#include "Nitrogen/Folders.hh"
#include "Nitrogen/OpenTransportProviders.hh"

// poseven
#include "poseven/bundles/inet.hh"
#include "poseven/functions/gethostname.hh"
#include "poseven/functions/perror.hh"
#include "poseven/functions/write.hh"
#include "poseven/types/exit_t.hh"

// FSContents
#include "FSContents.h"

// Arcana / SMTP
#include "SMTP.hh"

// Orion
#include "Orion/get_options.hh"
#include "Orion/Main.hh"


inline bool operator<( const InetMailExchange& a, const InetMailExchange& b )
{
	return a.preference < b.preference;
}


namespace tool
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace n = nucleus;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	namespace o = orion;
	
	using namespace io::path_descent_operators;
	
	
	static const char* gRelayServer = NULL;
	
	
	static N::FSDirSpec QueueDirectory()
	{
		return N::FSpMake_FSDirSpec( io::system_root< N::FSDirSpec >() / "j" / "var" / "spool" / "jmail" / "queue" );
	}
	
	
	static std::string DomainFromEmailAddress( const std::string& emailAddr )
	{
		std::string::size_type at = emailAddr.find( '@' );
		
		if ( at >= emailAddr.size() - 1 )
		{
			N::ThrowOSStatus( paramErr );  // bad email address
		}
		
		return emailAddr.substr( at + 1, emailAddr.find( '>' ) - (at + 1) );
	}
	
	static std::string OTLookup( const std::string& domain )
	{
	#if TARGET_RT_MAC_MACHO
		
		return "";
		
	#else
		
		std::vector< InetMailExchange > results;	
		
		results.resize( 10 );  // Should be more than enough
		
		N::OTInetMailExchange( NULL,
		                       (char*) domain.c_str(),
		                       results );
		
		std::sort( results.begin(),
		           results.end() );
		
		return results.front().exchange;
		
	#endif
	}
	
	static p7::in_addr_t ResolveHostname( const char* hostname )
	{
		hostent* hosts = gethostbyname( hostname );
		
		if ( !hosts || h_errno )
		{
			p7::perror( "sendmail: Domain name lookup failed", h_errno );
			
			throw p7::exit_failure;
		}
		
		in_addr addr = *(in_addr*) hosts->h_addr;
		
		return p7::in_addr_t( addr.s_addr );
	}
	
	static void Relay( const std::string&  returnPath,
	                   const std::string&  forwardPath,
	                   const FSSpec&       messageFile )
	{
		if ( forwardPath == "" )
		{
			// This will be the case if the dest file's data fork was empty,
			// which will be true of an Icon file.
			// The file will be deleted after we return.
			return;
		}
		
		std::printf( "Relaying from %s to %s\n", returnPath.c_str(), forwardPath.c_str() );
		
		std::string smtpServer;
		
		if ( gRelayServer != NULL )
		{
			smtpServer = gRelayServer;
			
			std::printf( "Using relay %s\n", gRelayServer );
		}
		else
		{
			std::string rcptDomain = DomainFromEmailAddress( forwardPath );
			
			//smtpServer = ResolverLookup( rcptDomain );
			smtpServer = OTLookup( rcptDomain );
			
			if ( !smtpServer.empty() )
			{
				std::printf( "MX for %s is %s\n", rcptDomain.c_str(), smtpServer.c_str() );
			}
			else
			{
				smtpServer = rcptDomain;
				
				std::printf( "Using domain %s as server\n", rcptDomain.c_str() );
			}
			
		}
		
		p7::in_port_t smtp_port = p7::in_port_t( 25 );
		
		p7::in_addr_t addr = ResolveHostname( smtpServer.c_str() );
		
		struct in_addr sin_addr = { addr };
		
		std::printf( "Address of %s is %s\n", smtpServer.c_str(), inet_ntoa( sin_addr ) );
		
		// Make a new socket
		// and connect to the server.  This could fail, thanks to a bunch of Cox.
		
		n::owned< p7::fd_t > smtp_server = p7::connect( addr, smtp_port );
		
		SMTP::Client::Session smtpSession( smtp_server );
		
		n::owned< N::FSFileRefNum > messageStream = io::open_for_reading( messageFile );
		
		smtpSession.Hello      ( p7::gethostname() );
		smtpSession.MailFrom   ( returnPath  );
		smtpSession.RecipientTo( forwardPath );
		
		smtpSession.BeginData();
		
		const std::size_t blockSize = 4096;
		
		char data[ blockSize ];
		
		while ( std::size_t bytes = io::read( messageStream, data, blockSize ) )
		{
			p7::write( smtp_server, data, bytes );
		}
		
		smtpSession.EndData();
		smtpSession.Quit();
	}
	
	static bool IsControlChar( char c )
	{
		return std::iscntrl( c );
	}
	
	template < class Stream >
	static std::string ReadOneLinerFromStream( Stream fileH )
	{
		std::string contents;
		
		contents.resize( io::get_file_size( fileH ) );
		
		io::read( fileH, &contents[0], contents.size() );
		
		const std::string::const_iterator end_of_first_line = std::find_if( contents.begin(),
		                                                                    contents.end(),
		                                                                    std::ptr_fun( IsControlChar ) );
		
		const std::size_t length_of_first_line = end_of_first_line - contents.begin();
		
		contents.resize( length_of_first_line );
		
		return contents;
	}
	
	template < class FileSpec >
	static inline std::string ReadOneLinerFromFile( const FileSpec& file )
	{
		return ReadOneLinerFromStream( io::open_for_reading( file ) );
	}
	
	
	class Transmitter
	{
		private:
			std::string  itsReturnPath;
			FSSpec       itsMessageFile;
			FSSpec       itsDestinations;
		
		public:
			Transmitter( const std::string&  returnPath,
			             const FSSpec&       message,
			             const FSSpec&       dests ) : itsReturnPath  ( returnPath ),
			                                           itsMessageFile ( message    ),
			                                           itsDestinations( dests      )
			{
			}
			
			void operator()( const unsigned char* dest_filename );
	};
	
	void Transmitter::operator()( const unsigned char* dest_filename )
	{
		try
		{
			FSSpec destFile = itsDestinations / dest_filename;
			
			// destFile serves as a lock on this destination
			// We can't switch from FSSpec to pathname until we sort out locking
			Relay( itsReturnPath,
			       ReadOneLinerFromStream( io::open_for_io( destFile ) ),
			       itsMessageFile );
			
			io::delete_file( destFile );
		}
		catch ( ... )
		{
			
		}
	}
	
	
	static void ProcessMessage( const N::FSDirSpec& messages, const unsigned char* name )
	{
		FSSpec msgFolderItem = messages / name;
		
		if ( !io::directory_exists( msgFolderItem ) )  return;  // Icon files, et al
		
		typedef io::filespec_traits< FSSpec >::optimized_directory_spec directory_spec;
		
		directory_spec msgFolder( N::FSpMake_FSDirSpec( msgFolderItem ) );
		
		FSSpec message      = msgFolder / "Message";
		FSSpec returnPath   = msgFolder / "Return-Path";
		FSSpec destinations = msgFolder / "Destinations";
		
		directory_spec destFolder( N::FSpMake_FSDirSpec( destinations ) );
		
		typedef io::directory_contents_traits< directory_spec >::container_type directory_container;
		directory_container contents = io::directory_contents( destFolder );
		
		std::for_each( contents.begin(),
		               contents.end(),
		               Transmitter( ReadOneLinerFromFile( returnPath ),
		                            message,
		                            destinations ) );
		
		io::delete_empty_directory( destFolder );  // this fails if destinations remain
		io::delete_file           ( returnPath );
		io::delete_file           ( message    );
		io::delete_empty_directory( msgFolder  );
	}
	
	
	int Main( int argc, iota::argv_t argv )
	{
		o::bind_option_to_variable( "--relay", gRelayServer );
		
		o::get_options( argc, argv );
		
		N::FSDirSpec queue_dir = QueueDirectory();
		
		N::FSSpecContents_Container queue = io::directory_contents( queue_dir );
		
		std::for_each( queue.begin(),
		               queue.end(),
		               std::bind1st( plus::ptr_fun( ProcessMessage ), queue_dir ) );
		
		return 0;
	}

}

