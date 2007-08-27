/*	=========
 *	macbin.cc
 *	=========
 */

// Standard C/C++
#include <cstdio>

// Standard C++
#include <functional>
#include <vector>

// POSIX
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

// Nucleus
#include "Nucleus/NAssert.h"

// Nitrogen
#include "Nitrogen/OSStatus.h"

// Divergence
#include "Divergence/Utilities.hh"

// Misc
#include "MacBinary.hh"

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

static void BlockWrite( int fd, const void* data, std::size_t byteCount )
{
	int written = write( fd, data, byteCount );
	
	if ( written < 0 )
	{
		std::fprintf( stderr, "macbin: Write error: %s\n", strerror( errno ) );
		
		O::ThrowExitStatus( 1 );
	}
}

	static void Decode( int input, const N::FSDirSpec& destDir )
	{
		MacBinary::Decoder decoder( destDir );
		
		char data[ 4096 ];
		
		std::size_t totalBytes = 0;
		
		try
		{
			while ( true )
			{
				std::size_t bytes = read( input, data, 4096 );
				
				if ( bytes == 0 )
				{
					break;
				}
				else if ( bytes < 0 )
				{
					std::fprintf( stderr, "macbin: Read failure: %s\n", strerror( errno ) );
					O::ThrowExitStatus( 1 );
				}
				
				decoder.Write( data, bytes );
				
				totalBytes += bytes;
			}
		}
		catch ( const io::end_of_input& )
		{
		}
		catch ( const MacBinary::InvalidMacBinaryHeader& )
		{
			std::fprintf( stderr, "Invalid MacBinary header somewhere past offset %x\n", totalBytes );
			O::ThrowExitStatus( 1 );
		}
	}
	
int O::Main( int argc, argv_t argv )
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
		
		const char* dest = argv[3];
		
		bool use_stdout = dest[0] == '-'  &&  dest[1] == '\0';
		
		int output = use_stdout ? 1 : open( dest, O_WRONLY | O_EXCL | O_CREAT, 0644 );
		
		if ( output < 0 )
		{
			std::fprintf( stderr, "macbin: Couldn't open %s: %s\n", dest, strerror( errno ) );
			
			O::ThrowExitStatus( 1 );
		}
		
		MacBinary::Encode( targetFile, &BlockWrite, output );
		
		close( output );
	}
	else if ( op == "--decode" )
	{
		// FIXME:  Can't decode from a stream
		const char* destDirPath = argv[ 3 ] ? argv[ 3 ] : ".";
		
		try
		{
			bool use_stdin = target[0] == '-'  &&  target[1] == '\0';
			
			int input = use_stdin ? 0 : open( target, O_RDONLY );
			
			if ( input < 0 )
			{
				std::fprintf( stderr, "macbin: Couldn't open %s: %s\n", target, strerror( errno ) );
				
				O::ThrowExitStatus( 1 );
			}
			
			Decode( input,
			        NN::Convert< N::FSDirSpec >( Div::ResolvePathToFSSpec( destDirPath ) ) );
			
			if ( !use_stdin )
			{
				close( input );
			}
		}
		catch ( const MacBinary::InvalidMacBinaryHeader& )
		{
			std::fprintf( stderr, "%s\n", "Invalid MacBinary header" );
			return 1;
		}
		catch ( const MacBinary::IncompatibleMacBinaryHeader& )
		{
			std::fprintf( stderr, "%s\n", "Incompatible (newer) MacBinary format." );
			return 2;
		}
	}
	else
	{
		return Usage();
	}
	
	return 0;
}

