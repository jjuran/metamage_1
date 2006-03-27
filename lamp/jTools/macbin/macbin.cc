/*	=========
 *	macbin.cc
 *	=========
 */

// Standard C++
#include <functional>
#include <vector>

// POSIX
#include "unistd.h"

// Nitrogen Nucleus
#include "Nucleus/NAssert.h"

// Nitrogen / Mac OS support
#include "Nitrogen/OSStatus.h"

// Divergence
#include "Divergence/Utilities.hh"

// Misc
#include "MacBinaryIII.hh"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"


namespace N = Nitrogen;
namespace NN = Nucleus;
namespace Div = Divergence;
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
	const char* target = argv[ 2 ];
	
	FSSpec targetFile = Div::ResolvePathToFSSpec( target );
	
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
		
		MacBinaryIII::Encode( targetFile,
		                      Div::ResolvePathToFSSpec( argv[ 3 ] ) );
	}
	else if ( op == "--decode" )
	{
		// FIXME:  Can't decode from a stream
		const char* destDirPath = argv[ 3 ] ? argv[ 3 ] : ".";
		
		MacBinaryIII::Decode( targetFile,
		                      NN::Convert< N::FSDirSpec >( Div::ResolvePathToFSSpec( destDirPath ) ) );
	}
	else
	{
		return Usage();
	}
	
	return 0;
}

