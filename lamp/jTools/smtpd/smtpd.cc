/*	========
 *	smtpd.cc
 *	========
 */

// Standard C/C++
#include <cstdlib>

// Standard C++
#include <algorithm>
#include <functional>
#include <list>
#include <sstream>
#include <string>
#include <vector>

// POSIX
#include "sys/socket.h"

// Nitrogen Nucleus
#include "Nucleus/Shared.h"

// Nitrogen / Carbon support
#include "Nitrogen/DateTimeUtils.h"
#include "Nitrogen/Folders.h"
#include "Nitrogen/OpenTransportProviders.h"

// Nitrogen Extras / Iteration
#include "Iteration/FSContents.h"

// Nitrogen Extras / Templates
#include "Templates/FunctionalExtensions.h"
#include "Templates/PointerToFunction.h"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"

// Io
#include "Io/Files.hh"
#include "Io/MakeHandle.hh"
#include "Io/TextInput.hh"

// BitsAndBytes
#include "DecimalStrings.hh"
#include "StringFilters.hh"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"


namespace N = Nitrogen;
namespace NN = Nucleus;
namespace O = Orion;

namespace ext = N::STLExtensions;

using std::string;
using std::vector;

using BitsAndBytes::EncodeDecimal2;
using BitsAndBytes::EncodeDecimal4;
using BitsAndBytes::q;


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
		if ( FSpTestDirectoryExists( item ) )
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
static string DateFormattedForFilename( unsigned long clock )
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

static string MakeMessageName()
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


static string GetForwardPath( const string& rcptLine )
{
	return rcptLine.substr( std::strlen( "RCPT TO:" ), string::npos );
}

static string GetReversePath( const string& fromLine )
{
	return fromLine.substr( std::strlen( "MAIL FROM:" ), string::npos );
}

static void CreateOneLiner( const FSSpec& file, const std::string& line )
{
	Io::S( N::FSpOpenDF( N::FSpCreate( file,
	                                   'R*ch',
	                                   'TEXT' ),
	                     fsWrPerm ) )
	<< line + "\n";
}

static void CreateDestinationFile( const N::FSDirSpec& destFolder, const std::string& dest )
{
	CreateOneLiner( destFolder & dest.substr( 0, 31 ),
	                dest );
}

static N::FSDirSpec QueueDirectory()
{
	return N::RootDirectory( N::BootVolume() ) << "j"
	                                           << "var"
	                                           << "spool"
	                                           << "jmail"
	                                           << "queue";
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
		void WriteLine( const string& line );
		
		void Finished();
};

PartialMessage::PartialMessage( const FSSpec& dirLoc )
:
	dir( NN::Owned< N::FSDirSpec, N::RecursiveFSDeleter >::Seize( N::FSpDirCreate( dirLoc ) ) ), 
	out( N::FSpOpenDF( N::FSpCreate( dir.Get() & "Message",
	                                 'R*ch',
	                                 'TEXT'),
	                   fsWrPerm ) )
{
	//
}

void PartialMessage::WriteLine( const string& line )
{
	//static unsigned int lastFlushKBytes = 0;
	string terminatedLine = line + "\r\n";
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


string myHello;
string myFrom;
std::list< string > myTo;
PartialMessage myMessage;
bool dataMode = false;


static void QueueMessage()
{
	N::FSDirSpec dir = myMessage.Dir();
	
	// Create the Destinations subdirectory.
	N::FSDirSpec destFolder = N::FSpDirCreate( dir & "Destinations" );
	
	// Create the destination files.
	std::for_each( myTo.begin(),
	               myTo.end(),
	               ext::compose1( std::bind1st( N::PtrFun( CreateDestinationFile ),
	                                            destFolder ),
	                              N::PtrFun( GetForwardPath ) ) );
	
	// Create the Return-Path file.
	// Write this last so the sender won't delete the message prematurely.
	CreateOneLiner( dir & "Return-Path", 
	                GetReversePath( myFrom ) );
	
}

static void DoCommand( const string& command )
{
	string word = command.substr( 0, command.find(' ') );
	
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
		PartialMessage msg( QueueDirectory() & MakeMessageName() );
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

static void DoData( const string& data )
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

static void DoLine( const string& line )
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

int O::Main( int argc, char const *const argv[] )
{
	InetAddress peer;
	socklen_t peerlen = sizeof peer;
	
	if ( getpeername( 0, (sockaddr*)&peer, &peerlen ) == 0 )
	{
		Io::Err << "Connection from "
		        << N::OTInetHostToString( peer.fHost )
		        << ", port "
		        << peer.fPort
		        << ".\n";
	}
	
	Io::TextInputAdapter input( Io::MakeHandleFromCast< Io::FD_Details >( Io::in ) );
	
	const char* hostname = "temporarily.out.of.order";
	
	Io::Out << "220 " << hostname << " ready"  "\r\n";
	
	while ( input.Ready() )
	{
		DoLine( input.Read() );
	}
	
	return 0;
}

