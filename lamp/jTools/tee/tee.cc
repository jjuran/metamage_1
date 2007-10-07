/*	======
 *	tee.cc
 *	======
 */

// POSIX
#include <fcntl.h>
#include <unistd.h>

// POSeven
#include "POSeven/Errno.hh"
#include "POSeven/FileDescriptor.hh"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"


namespace p7 = poseven;
namespace O = Orion;


int O::Main( int argc, argv_t argv )
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
			enum { kDataSize = 4096 };
			
			char data[ kDataSize ];
			
			int bytes = io::read( p7::stdin_fileno, data, kDataSize );
			
			(void) write( output, data, bytes );  // FIXME:  check for errors
			(void) write( stdOut, data, bytes );
		}
	}
	catch ( const io::end_of_input& )
	{
		// We're done
	}
	catch ( const p7::errno_t& err )
	{
		return err.Get() != 0;
	}
	
	return 0;
}

