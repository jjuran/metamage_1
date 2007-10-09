/*	======
 *	tee.cc
 *	======
 */

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Iota
#include "iota/strings.hh"

// POSeven
#include "POSeven/Errno.hh"
#include "POSeven/Open.hh"

// Orion
#include "Orion/Main.hh"


namespace p7 = poseven;
namespace O = Orion;


int O::Main( int argc, argv_t argv )
{
	// Check for correct number of args
	if ( argc != 2 )
	{
		p7::write( p7::stderr_fileno, STR_LEN( "tee: needs one argument\n" ) );
		
		return 1;
	}
	
	const char* const outFile = argv[ 1 ];
	
	const p7::fd_t output = open( outFile, p7::o_wronly ).release();  // Will be closed when we exit
	
	const std::size_t blockSize = 4096;
	
	char data[ blockSize ];
	
	try
	{
		while ( std::size_t bytes = p7::read( p7::stdin_fileno, data, blockSize ) )
		{
			(void) p7::write( output,            data, bytes );
			(void) p7::write( p7::stdout_fileno, data, bytes );
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

