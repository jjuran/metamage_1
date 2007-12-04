/*	=========
 *	md5sum.cc
 *	=========
 */

// Standard C++
#include <string>

// POSeven
#include "POSeven/Open.hh"

// BitsAndBytes
#include "HexStrings.hh"

// Arcana
#include "MD5.hh"

// Orion
#include "Orion/Main.hh"


namespace p7 = poseven;
namespace O = Orion;
namespace Bits = BitsAndBytes;


static std::string MD5Sum( p7::fd_t input )
{
	const std::size_t blockSize = 64;
	
	char data[ blockSize ];
	std::size_t bytes;
	MD5::Engine engine;
	
	// loop exits on a partial block or at EOF
	while ( ( bytes = p7::read( input, data, blockSize ) ) == blockSize )
	{
		engine.DoBlock( data );
	}
	
	engine.Finish( data, bytes * 8 );
	
	std::string digest = Bits::EncodeAsHex( engine.GetResult() );
	
	return digest;
}

int O::Main( int argc, argv_t argv )
{
	int fail = 0;
	
	for ( int i = 1;  i < argc;  ++i )
	{
		try
		{
			std::printf( "%s  %s\n",
			              MD5Sum( p7::open( argv[ i ], p7::o_rdonly ) ),
			                  argv[ i ] );
		}
		catch ( ... )
		{
			fail++;
		}
	}
	
	return fail == 0 ? 0 : 1;
}

