/*	======
 *	cat.cc
 *	======
 */

// POSIX
#include "fcntl.h"
#include "unistd.h"

// Nitrogen
#include "Nitrogen/OSStatus.h"
#include "Nitrogen/Threads.h"

// Io
#include "Io/Exceptions.hh"
#include "Io/MakeHandle.hh"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"


namespace N = Nitrogen;
namespace O = Orion;


static int DumpFile( const Io::Handle& io );

static bool PathnameMeansStdIn( const std::string& pathname )
{
	return pathname == "-";
}

int O::Main( int argc, const char *const argv[] )
{
	// Check for sufficient number of args
	if ( argc < 2 )
	{
		Io::Err << "cat: missing arguments\n";
		return 1;
	}
	
	// Print each file in turn.  Return whether any errors occurred.
	int fail = 0;
	
	for ( int index = 1; index < argc; index++ )
	{
		const char* pathname = argv[ index ];
		
		int fd = PathnameMeansStdIn( pathname ) ? 0
		                                        : open( pathname, 0 );
		
		if ( fd == -1 )
		{
			//Io::Err << "Error " << errno << " opening file " << pathname << "\n";
			Io::Err << "cat: " << pathname << ": No such file or directory\n";
			continue;
		}
		
		Io::Handle in = Io::MakeHandleFromCast< Io::FD_Details, Io::FD >( Io::FD( fd ) );
		
		try
		{
			fail += DumpFile( in );
		}
		catch ( ... )
		{
			Io::Err << "cat: " << pathname << ": DumpFile() failed\n";
			fail++;
		}
		
		if ( fd != 0 )
		{
			(void)close( fd );
		}
	}
	
	return (fail == 0) ? 0 : 1;
}

int DumpFile( const Io::Handle& in )
{
	while ( true )
	{
		try
		{
			enum { blockSize = 512 };
			char data[ blockSize ];
			
			int bytes = in.Read( data, blockSize );
			
			Io::Put( Io::Out, data, bytes );
		}
		catch ( Io::EndOfInput err )
		{
			break;
		}
		catch ( Io::NoDataPending )
		{
			sleep( 0 );
		}
		catch ( N::OSStatus err )
		{
			Io::Err << "cat: OSStatus " << err << "\n";
			return 1;
		}
	}
	
	return 0;
}
