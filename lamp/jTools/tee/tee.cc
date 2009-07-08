/*	======
 *	tee.cc
 *	======
 */

// Iota
#include "iota/strings.hh"

// POSeven
#include "POSeven/functions/open.hh"
#include "POSeven/functions/read.hh"
#include "POSeven/functions/write.hh"

// Orion
#include "Orion/Main.hh"


namespace tool
{
	
	namespace p7 = poseven;
	
	int Main( int argc, iota::argv_t argv )
	{
		// Check for correct number of args
		if ( argc != 2 )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "tee: needs one argument\n" ) );
			
			return 1;
		}
		
		const char* const outFile = argv[ 1 ];
		
		const p7::fd_t output = p7::open( outFile, p7::o_wronly ).release();  // Will be closed when we exit
		
		const std::size_t blockSize = 4096;
		
		char data[ blockSize ];
		
		while ( std::size_t bytes = p7::read( p7::stdin_fileno, data, blockSize ) )
		{
			(void) p7::write( output,            data, bytes );
			(void) p7::write( p7::stdout_fileno, data, bytes );
		}
		
		return 0;
	}
	
}

