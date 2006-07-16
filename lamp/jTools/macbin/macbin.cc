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
#include "unistd.h"

// Nitrogen Nucleus
#include "Nucleus/NAssert.h"

// Nitrogen
#include "Nitrogen/OSStatus.h"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"

// Divergence
#include "Divergence/Utilities.hh"

// Misc
#include "MacBinary.hh"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"


namespace N = Nitrogen;
namespace NN = Nucleus;
namespace FS = FileSystem;
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

	static void Encode( const FSSpec& source, const FSSpec& dest )
	{
		MacBinary::Encoder encoder( source );
		
		N::FSpCreate( dest, '????', 'BINA' );
		
		NN::Owned< N::FSFileRefNum > out = N::FSpOpenDF( dest, fsWrPerm );
		
		char data[ 4096 ];
		
		try
		{
			while ( true )
			{
				std::size_t bytes = encoder.Read( data, 4096 );
				
				FS::Write( out, data, bytes );
			}
		}
		catch ( FS::EndOfFile& ) {}
	}
	
	static void Decode( const FSSpec& source, const N::FSDirSpec& destDir )
	{
		NN::Owned< N::FSFileRefNum > in = N::FSpOpenDF( source, fsRdPerm );
		
		MacBinary::Decoder decoder( destDir );
		
		char data[ 4096 ];
		
		std::size_t totalBytes = 0;
		
		try
		{
			while ( true )
			{
				std::size_t bytes = FS::Read( in, data, 4096 );
				
				decoder.Write( data, bytes );
				
				totalBytes += bytes;
				
				sleep( 0 );
			}
		}
		catch ( const FS::EndOfFile& )
		{
		}
		catch ( const MacBinary::InvalidMacBinaryHeader& )
		{
			std::fprintf( stderr, "Invalid MacBinary header somewhere past offset %x\n", totalBytes );
			O::ThrowExitStatus( 1 );
		}
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
		
		Encode( targetFile,
		        Div::ResolvePathToFSSpec( argv[ 3 ] ) );
	}
	else if ( op == "--decode" )
	{
		// FIXME:  Can't decode from a stream
		const char* destDirPath = argv[ 3 ] ? argv[ 3 ] : ".";
		
		try
		{
			Decode( targetFile,
			        NN::Convert< N::FSDirSpec >( Div::ResolvePathToFSSpec( destDirPath ) ) );
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

