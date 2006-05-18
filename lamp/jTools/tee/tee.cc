/*	======
 *	tee.cc
 *	======
 */

// POSIX
#include "fcntl.h"
#include "unistd.h"

// Io
#include "Io/Exceptions.hh"

// POSeven
#include "POSeven/Errno.hh"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"


namespace P7 = POSeven;
namespace O = Orion;


int O::Main( int argc, const char *const argv[] )
{
	// Check for correct number of args
	if ( argc != 2 )
	{
		Io::Err << "tee: needs one argument\n";
		return 1;
	}
	
	const char* const outFile = argv[ 1 ];
	
	const int output = open( outFile, O_WRONLY );  // Will be closed when we exit
	const int stdOut = 1;
	
	try
	{
		while ( true )
		{
			enum { kDataSize = 512 };
			
			char data[ kDataSize ];
			
			int bytes = Io::Read( Io::in, data, kDataSize );
			
			/*
			Io::Put( Io::S( gOut ), data, bytes );
			Io::Put( Io::Out,       data, bytes );
			*/
			
			(void)write( output, data, bytes );  // FIXME:  check for errors
			(void)write( stdOut, data, bytes );
		}
	}
	catch ( Io::EndOfInput )
	{
		// We're done
	}
	catch ( P7::Errno& err )
	{
		return err.Get() != 0;
	}
	
	return 0;
}

