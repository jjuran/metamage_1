/*	=========
 *	macbin.cc
 *	=========
 */

// Standard C++
#include <functional>
#include <vector>

// POSIX
#include "unistd.h"

// Nitrogen core
#include "Nitrogen/Assert.h"

// Nitrogen / Mac OS support
#include "Nitrogen/OSStatus.h"

// Misc
#include "MacBinaryIII.hh"

// Kerosene
#include "SystemCalls.hh"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"


namespace N = Nitrogen;
namespace O = Orion;


// Operations:
// 
// Foo must be a real (possibly nonexisting) file (not a device stream)
// 
// --encode Foo foo.out   # If foo.out is omitted, a name is chosen.
// --decode foo.in Foo    # If Foo is omitted, the MacBinary-specified name is used.
// --check foo.in         # tests whether or not foo.in is a MacBinary envelope.


static int Usage()
{
	Io::Err << "Usage: macbin --encode Source dest.bin\n"
	           "       macbin --decode source.bin [dest-dir]\n";
	
	return 1;
}

int O::Main( int argc, const char *const argv[] )
{
	if ( argc < 3 )
	{
		return Usage();
	}
	
	std::string op     = argv[ 1 ];
	std::string target = argv[ 2 ];
	
	FSSpec file = Path2FSS( target );
	
	if (false)
	{
	}
	else if ( op == "--encode" )
	{
		// FIXME:  Can't encode to a stream
		if ( argv[ 3 ] == NULL )
		{
			return Usage();
		}
		
		MacBinaryIII::Encode( Path2FSS( target ), Path2FSS( argv[ 3 ] ) );
	}
	else if ( op == "--decode" )
	{
		// FIXME:  Can't decode from a stream
		std::string destDirPath = argv[ 3 ] ? argv[ 3 ] : ".";
		
		MacBinaryIII::Decode( Path2FSS( target ),
		                      N::Convert< N::FSDirSpec >( Path2FSS( destDirPath ) ) );
	}
	else
	{
		return Usage();
	}
	
	return 0;
}

