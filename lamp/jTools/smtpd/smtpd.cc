/*	========
 *	smtpd.cc
 *	========
 */

// Standard C++
#include <algorithm>
#include <functional>
#include <list>
#include <sstream>
#include <string>
#include <vector>

// Standard C/C++
#include <cstdlib>

// POSIX
#include <arpa/inet.h>
#include <sys/socket.h>

// Nucleus
#include "Nucleus/Shared.h"

// Io
#include "io/io.hh"
#include "io/spray.hh"

// Nitrogen
#include "Nitrogen/DateTimeUtils.h"
#include "Nitrogen/Folders.h"

// POSeven
#include "POSeven/FileDescriptor.hh"

// Nitrogen Extras / Iteration
#include "Iteration/FSContents.h"

// Nitrogen Extras / Templates
#include "Templates/FunctionalExtensions.h"
#include "Templates/PointerToFunction.h"

// Io
#include "Io/TextInput.hh"

// BitsAndBytes
#include "DecimalStrings.hh"
#include "StringFilters.hh"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"


namespace N = Nitrogen;
namespace NN = Nucleus;
namespace P7 = POSeven;
namespace O = Orion;

namespace ext = N::STLExtensions;

using namespace io::path_descent_operators;

using BitsAndBytes::EncodeDecimal2;
using BitsAndBytes::EncodeDecimal4;
using BitsAndBytes::q;


namespace io
{
	template < class FileSpec > struct system_root_directory;
	
	template < class FileSpec >
	inline FileSpec system_root()
	{
		return system_root_directory< FileSpec >()();
	}
	
	template <> struct system_root_directory< Nitrogen::FSVolumeRefNum >
	{
		Nitrogen::FSVolumeRefNum operator()() const
		{
			return Nitrogen::FindFolder( Nitrogen::kOnSystemDisk,
			                             Nitrogen::kSystemFolderType,
			                             kDontCreateFolder ).vRefNum;
		}
	};
	
	template <> struct system_root_directory< Nitrogen::FSDirSpec >
	{
		Nitrogen::FSDirSpec operator()() const
		{
			return Nucleus::Make< Nitrogen::FSDirSpec >( system_root< Nitrogen::FSVolumeRefNum >(),
			                                             Nitrogen::fsRtDirID );
		}
	};
	
	template <> struct system_root_directory< FSSpec >
	{
		FSSpec operator()() const
		{
			return Nucleus::Convert< FSSpec >( system_root< Nitrogen::FSDirSpec >() );
		}
	};
	
}

namespace Nitrogen
{
	
	static void RecursivelyDelete( const FSSpec& item );
	
	static void RecursivelyDeleteDirectoryContents( const FSDirSpec& dir )
	{
		std::for_each( FSContents( dir ).begin(),
		               FSContents( dir ).end(),
		               std::ptr_fun( RecursivelyDelete ) );
	}
	
	static void RecursivelyDelete( const FSSpec& item )
	{
		if ( io::directory_exists( item ) )
		{
			RecursivelyDeleteDirectoryContents( Nucleus::Convert< FSDirSpec >( item ) );
		}
		
		FSpDelete( item );
	}
	
	struct RecursiveFSDeleter
	{
		void operator()( const FSDirSpec& dir ) const
		{
			RecursivelyDelete( Nucleus::Convert< FSSpec >( dir ) );
		}
	};
	
}

namespace Nucleus
{
	
	template <>
	struct LivelinessTraits< Nitrogen::FSDirSpec, Nitrogen::RecursiveFSDeleter >   { typedef SeizedValuesAreLive LivelinessTest; };
	
}


// E.g. "19840124.183000"
static std::string DateFormattedForFilename( unsigned long clock )
{
	DateTimeRec date = N::SecondsToDate( clock );
	
	std::ostringstream stamp;
	
	stamp << EncodeDecimal4( date.year   )
	      << EncodeDecimal2( date.month  )
	      << EncodeDecimal2( date.day    )
	      << "."
	      << EncodeDecimal2( date.hour   )
	      << EncodeDecimal2( date.minute )
	      << EncodeDecimal2( date.second );
	
	return stamp.str();
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
	
	return DateFormattedForFilename( now ) + "-" + EncodeDecimal2( serial );
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
	
	io::spray_file< Flattener >( N::FSpCreate( file,
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
	return N::FSDirSpec( io::system_root< N::FSDirSpec >() / "j" / "var" / "spool" / "jmail" / "queue" );
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
	Io::S( out ) << terminatedLine;
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
	               ext::compose1( std::bind1st( N::PtrFun( CreateDestinationFile ),
	                                            destFolder ),
	                              N::PtrFun( GetForwardPath ) ) );
	
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
		Io::Out << "250 Sender ok, probably"  "\r\n";
	}
	else if ( word == "RCPT" )
	{
		myTo.push_back( command );
		Io::Out << "250 Recipient ok, I guess"  "\r\n";
	}
	else if ( word == "DATA" )
	{
		PartialMessage msg( QueueDirectory() / MakeMessageName() );
		myMessage = msg;
		dataMode = true;
		Io::Out << "354 I'm listening"  "\r\n";
	}
	else if ( word == "HELO" )
	{
		myHello = command;
		Io::Out << "250 Hello there"  "\r\n";
	}
	else if ( word == "RSET" )
	{
		myFrom = "";
		myTo.clear();
		//myData.clear();
		Io::Out << "250 Reset"  "\r\n";
	}
	else if ( word == "QUIT" )
	{
		//isComplete = true;
		Io::Out << "221 Closing connection"  "\r\n";
		
		O::ThrowExitStatus( 0 );
	}
	else
	{
		if ( word != "EHLO" )
		{
			Io::Err << "Unrecognized command " << q( word ) << "\n";
		}
		
		Io::Out << "500 Unrecognized command"  "\r\n";
	}
}

static void DoData( const std::string& data )
{
	myMessage.WriteLine( data );
	
	if ( data == "." )
	{
		Io::Err << "done\n";
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
		
		Io::Out << ( queued ? "250 Message accepted"      "\r\n" 
		                    : "554 Can't accept message"  "\r\n" );
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

int O::Main( int /*argc*/, char const *const /*argv*/[] )
{
	sockaddr_in peer;
	socklen_t peerlen = sizeof peer;
	
	if ( getpeername( 0, (sockaddr*)&peer, &peerlen ) == 0 )
	{
		Io::Err << "Connection from "
		        << inet_ntoa( peer.sin_addr )
		        << ", port "
		        << peer.sin_port
		        << ".\n";
	}
	
	Io::TextInputAdapter< P7::FileDescriptor > input( P7::kStdIn_FileNo );
	
	const char* hostname = "temporarily.out.of.order";
	
	Io::Out << "220 " << hostname << " ready"  "\r\n";
	
	while ( input.Ready() )
	{
		DoLine( input.Read() );
	}
	
	return 0;
}

