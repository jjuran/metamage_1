/*	===========
 *	sendmail.cc
 *	===========
 */

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// mac-config
#include "mac_config/open-transport.hh"

// Mac OS
#if CONFIG_OPEN_TRANSPORT_HEADERS
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

// Standard C++
#include <algorithm>

// iota
#include "iota/char_types.hh"

// command
#include "command/get_option.hh"

// mac-types
#include "mac_types/VRefNum_DirID.hh"

// mac-file-utils
#include "mac_file/boot_volume.hh"
#include "mac_file/directory.hh"
#include "mac_file/listing.hh"
#include "mac_file/open_data_fork.hh"
#include "mac_file/rw.hh"
#include "mac_file/scoped_open_refnum.hh"

// plus
#include "plus/string.hh"

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"

// Io: MacFiles
#include "MacFiles/Classic.hh"

// poseven
#include "poseven/bundles/inet.hh"
#include "poseven/functions/gethostname.hh"
#include "poseven/functions/perror.hh"
#include "poseven/functions/write.hh"
#include "poseven/types/exit_t.hh"

// Arcana / SMTP
#include "SMTP.hh"

// Orion
#include "Orion/Main.hh"


#define DESTINATIONS  "Destinations"


#if CONFIG_OPEN_TRANSPORT_HEADERS

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
	namespace p7 = poseven;
	
	using mac::file::FSIORefNum;
	using mac::file::open_data_fork;
	using mac::file::scoped_open_refnum;
	
	using namespace io::path_descent_operators;
	
	using mac::types::VRefNum_DirID;
	
	
	static inline
	long directory_ID( const VRefNum_DirID& dir, const Byte* name )
	{
		return dir.vRefNum ? mac::file::directory( dir, name ).dirID
		                   : dir.dirID;
	}
	
	static inline
	VRefNum_DirID
	QueueDirectory()
	{
		VRefNum_DirID folder;
		
		folder.vRefNum = mac::file::boot_volume();
		folder.dirID   = fsRtDirID;
		
		folder.dirID = directory_ID( folder, "\p" "j"     );
		folder.dirID = directory_ID( folder, "\p" "var"   );
		folder.dirID = directory_ID( folder, "\p" "spool" );
		folder.dirID = directory_ID( folder, "\p" "jmail" );
		folder.dirID = directory_ID( folder, "\p" "queue" );
		
		return folder;
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
		
		InetMailExchange* min = std::min_element( &results[ 0 ],
		                                          &results[ n ] );
		
		return min->exchange;
		
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
	                   FSIORefNum           message )
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
		
		smtpSession.Hello      ( p7::gethostname() );
		smtpSession.MailFrom   ( returnPath  );
		smtpSession.RecipientTo( forwardPath );
		
		smtpSession.BeginData();
		
		const size_t blockSize = 4096;
		
		char data[ blockSize ];
		
		ssize_t bytes;
		
		while ( (bytes = mac::file::read( message, data, blockSize )) > 0 )
		{
			p7::write( smtp_server, data, bytes );
		}
		
		if ( bytes < 0 )
		{
			Mac::ThrowOSStatus( bytes );
		}
		
		smtpSession.EndData();
		smtpSession.Quit();
	}
	
	static
	plus::string ReadOneLinerFromStream( FSIORefNum fileH )
	{
		if ( fileH < 0 )
		{
			Mac::ThrowOSStatus( fileH );
		}
		
		plus::string contents;
		
		size_t length_of_first_line = 0;
		
		OSErr err = fileH;
		
		if ( fileH >= 0 )
		{
			Size size;
			char* p;
			
			(err = GetEOF( fileH, &size ))                                    ||
			(! (p = contents.reset_nothrow( size ))  &&  (err = memFullErr))  ||
			(err = mac::file::read_all( fileH, p, size ));
			
			FSClose( fileH );
			
			char* q = p;
			
			while ( ! iota::is_cntrl( *q++ ) )  continue;
			
			const char* end_of_first_line = --q;
			
			length_of_first_line = end_of_first_line - p;
		}
		
		Mac::ThrowOSStatus( err );
		
		return contents.substr( 0, length_of_first_line );
	}
	
	
	static
	void ProcessMessage( const VRefNum_DirID& messages, const Byte* name )
	{
		using mac::file::directory;
		using mac::file::directory_listing;
		using mac::file::list_entry;
		
		short vRefNum = messages.vRefNum;
		
		long message_dirID = directory( vRefNum, messages.dirID, name ).dirID;
		
		if ( message_dirID < 0 )  return;  // Icon files, et al
		
		Mac::FSDirSpec msgFolder;
		Mac::FSDirSpec destFolder;
		
		msgFolder.vRefNum = Mac::FSVolumeRefNum( vRefNum       );
		msgFolder.dirID   = Mac::FSDirID       ( message_dirID );
		
		FSSpec message      = msgFolder / "\p" "Message";
		FSSpec returnPath   = msgFolder / "\p" "Return-Path";
		
		long dest_dirID = directory( vRefNum,
		                             message_dirID,
		                             "\p" DESTINATIONS ).dirID;
		
		destFolder.vRefNum = Mac::FSVolumeRefNum( vRefNum    );
		destFolder.dirID   = Mac::FSDirID       ( dest_dirID );
		
		directory_listing listing;
		
		OSErr err = list_directory( listing, vRefNum, dest_dirID );
		
		Mac::ThrowOSStatus( err );
		
		plus::string return_path = ReadOneLinerFromStream( open_data_fork( returnPath, fsRdPerm ) );
		
		unsigned n = listing.count();
		
		for ( unsigned i = 0;  i < n;  ++i )
		{
			scoped_open_refnum opened( open_data_fork( message, fsRdPerm ) );
			
			if ( opened < 0 )
			{
				continue;
			}
			
			const list_entry& entry = listing.get_nth( i );
			
			try
			{
				FSSpec destFile = destFolder / entry.name;
				
				/*
					destFile serves as a lock on this destination.
					
					We can't switch from HFS-based filing to pathnames
					until we sort out file locking.
				*/
				
				Relay( return_path,
				       ReadOneLinerFromStream( open_data_fork( destFile, fsRdWrPerm ) ),
				       opened );
				
				io::delete_file( destFile );
			}
			catch ( ... )
			{
			}
		}
		
		io::delete_empty_directory( destFolder );  // this fails if destinations remain
		io::delete_file           ( returnPath );
		io::delete_file           ( message    );
		io::delete_empty_directory( msgFolder  );
	}
	
	
	int Main( int argc, char** argv )
	{
		using mac::file::directory_listing;
		using mac::file::list_entry;
		
		char *const *args = get_options( argv );
		
		const int argn = argc - (args - argv);
		
		VRefNum_DirID queue_dir = QueueDirectory();
		
		short vRefNum = queue_dir.vRefNum;
		long  dirID   = queue_dir.dirID;
		
		directory_listing listing;
		
		OSErr err = list_directory( listing, vRefNum, dirID );
		
		Mac::ThrowOSStatus( err );
		
		unsigned n = listing.count();
		
		for ( unsigned i = 0;  i < n;  ++i )
		{
			const list_entry& entry = listing.get_nth( i );
			
			ProcessMessage( queue_dir, entry.name );
		}
		
		return 0;
	}

}
