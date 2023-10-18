/*	===========
 *	sendmail.cc
 *	===========
 */

// Standard C++
#include <algorithm>

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef MAC_OS_X_VERSION_10_8
// OpenTransportProviders.h depends on FSSpec but doesn't include Files.h.
#ifndef __FILES__
#include <Files.h>
#endif
#ifndef __OPENTRANSPORT__
#include <OpenTransport.h>
#endif
#ifndef __OPENTRANSPORTPROVIDERS__
#include <OpenTransportProviders.h>
#endif
#endif

// POSIX
#include <arpa/inet.h>
#include <netdb.h>

// Standard C
#include <stdio.h>
#include <stdlib.h>

// iota
#include "iota/char_types.hh"

// command
#include "command/get_option.hh"

// plus
#include "plus/string.hh"

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"

#include "Nitrogen/Folders.hh"

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
#include "Orion/Main.hh"


#ifndef MAC_OS_X_VERSION_10_8

inline bool operator<( const InetMailExchange& a, const InetMailExchange& b )
{
	return a.preference < b.preference;
}

static inline
UInt16 OTInetMailExchange( InetSvcRef         ref,
                           char*              name,
                           UInt16             num,
                           InetMailExchange*  mx )
{
	Mac::ThrowOSStatus( OTInetMailExchange( ref, name, &num, mx ) );
	
	return num;
}

#endif


using namespace command::constants;

enum
{
	Option_last_byte = 255,
	
	Option_relay,
};

static command::option options[] =
{
	{ "relay", Option_relay },
	
	{ NULL }
};

static const char* gRelayServer = NULL;

static char* const* get_options( char* const* argv )
{
	++argv;  // skip arg 0
	
	short opt;
	
	while ( (opt = command::get_option( &argv, options )) )
	{
		switch ( opt )
		{
			case Option_relay:
				gRelayServer = command::global_result.param;
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
	namespace N = Nitrogen;
	namespace n = nucleus;
	namespace p7 = poseven;
	
	using namespace io::path_descent_operators;
	
	
	static N::FSDirSpec QueueDirectory()
	{
		Mac::FSDirSpec folder = N::FindFolder( N::kOnSystemDisk,
		                                       N::kSystemFolderType,
		                                       kDontCreateFolder );
		
		folder.dirID = Mac::fsRtDirID;  // root
		
		return N::FSpMake_FSDirSpec( folder / "j" / "var" / "spool" / "jmail" / "queue" );
	}
	
	
	static plus::string DomainFromEmailAddress( const plus::string& emailAddr )
	{
		plus::string::size_type at = emailAddr.find( '@' );
		
		if ( at >= emailAddr.size() - 1 )
		{
			Mac::ThrowOSStatus( paramErr );  // bad email address
		}
		
		return emailAddr.substr( at + 1, emailAddr.find( '>' ) - (at + 1) );
	}
	
	static plus::string OTLookup( const plus::string& domain )
	{
	#if TARGET_RT_MAC_MACHO
		
		return "";
		
	#else
		
		const int max_results = 10;  // Should be more than enough
		
		InetMailExchange results[ max_results ];
		
		UInt16 n = OTInetMailExchange( NULL,
		                               (char*) domain.c_str(),
		                               max_results,
		                               &results[ 0 ] );
		
		std::sort( &results[ 0 ],
		           &results[ n ] );
		
		return results[ 0 ].exchange;
		
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
	
	static void Relay( const plus::string&  returnPath,
	                   const plus::string&  forwardPath,
	                   const FSSpec&        messageFile )
	{
		if ( forwardPath == "" )
		{
			// This will be the case if the dest file's data fork was empty,
			// which will be true of an Icon file.
			// The file will be deleted after we return.
			return;
		}
		
		printf( "Relaying from %s to %s\n", returnPath.c_str(), forwardPath.c_str() );
		
		plus::string smtpServer;
		
		if ( gRelayServer != NULL )
		{
			smtpServer = gRelayServer;
			
			printf( "Using relay %s\n", gRelayServer );
		}
		else
		{
			plus::string rcptDomain = DomainFromEmailAddress( forwardPath );
			
			//smtpServer = ResolverLookup( rcptDomain );
			smtpServer = OTLookup( rcptDomain );
			
			if ( !smtpServer.empty() )
			{
				printf( "MX for %s is %s\n", rcptDomain.c_str(), smtpServer.c_str() );
			}
			else
			{
				smtpServer = rcptDomain;
				
				printf( "Using domain %s as server\n", rcptDomain.c_str() );
			}
			
		}
		
		p7::in_port_t smtp_port = p7::in_port_t( 25 );
		
		p7::in_addr_t addr = ResolveHostname( smtpServer.c_str() );
		
		struct in_addr sin_addr = { addr };
		
		printf( "Address of %s is %s\n", smtpServer.c_str(), inet_ntoa( sin_addr ) );
		
		// Make a new socket
		// and connect to the server.  This could fail, thanks to a bunch of Cox.
		
		n::owned< p7::fd_t > smtp_server = p7::connect( addr, smtp_port );
		
		SMTP::Client::Session smtpSession( smtp_server );
		
		n::owned< N::FSFileRefNum > messageStream = io::open_for_reading( messageFile );
		
		smtpSession.Hello      ( p7::gethostname() );
		smtpSession.MailFrom   ( returnPath  );
		smtpSession.RecipientTo( forwardPath );
		
		smtpSession.BeginData();
		
		const size_t blockSize = 4096;
		
		char data[ blockSize ];
		
		while ( size_t bytes = io::read( messageStream, data, blockSize ) )
		{
			p7::write( smtp_server, data, bytes );
		}
		
		smtpSession.EndData();
		smtpSession.Quit();
	}
	
	template < class Stream >
	static plus::string ReadOneLinerFromStream( Stream fileH )
	{
		const size_t file_size = io::get_file_size( fileH );
		
		plus::string contents;
		
		char* p = contents.reset( file_size );
		
		io::read( fileH, p, file_size );
		
		char* q = p;
		
		while ( ! iota::is_cntrl( *q++ ) )  continue;
		
		--q;
		
		const char* end_of_first_line = q;
		
		const size_t length_of_first_line = end_of_first_line - p;
		
		return contents.substr( 0, length_of_first_line );
	}
	
	template < class FileSpec >
	static inline plus::string ReadOneLinerFromFile( const FileSpec& file )
	{
		return ReadOneLinerFromStream( io::open_for_reading( file ) );
	}
	
	
	class Transmitter
	{
		private:
			plus::string  itsReturnPath;
			FSSpec        itsMessageFile;
			FSSpec        itsDestinations;
		
		public:
			Transmitter( const plus::string&  returnPath,
			             const FSSpec&        message,
			             const FSSpec&        dests )
			:
				itsReturnPath  ( returnPath ),
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
		
		directory_container dests = io::directory_contents( destFolder );
		
		typedef directory_container::const_iterator Iter;
		
		Transmitter transmitter( ReadOneLinerFromFile( returnPath ),
		                         message,
		                         destinations );
		
		for ( Iter it = dests.begin(), end = dests.end();  it != end;  ++it )
		{
			transmitter( *it );
		}
		
		io::delete_empty_directory( destFolder );  // this fails if destinations remain
		io::delete_file           ( returnPath );
		io::delete_file           ( message    );
		io::delete_empty_directory( msgFolder  );
	}
	
	
	int Main( int argc, char** argv )
	{
		char *const *args = get_options( argv );
		
		const int argn = argc - (args - argv);
		
		N::FSDirSpec queue_dir = QueueDirectory();
		
		N::FSSpecContents_Container queue = io::directory_contents( queue_dir );
		
		typedef N::FSSpecContents_Container::const_iterator Iter;
		
		for ( Iter it = queue.begin(), end = queue.end();  it != end;  ++it )
		{
			ProcessMessage( queue_dir, *it );
		}
		
		return 0;
	}

}
