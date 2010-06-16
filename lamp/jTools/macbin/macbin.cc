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

// Iota
#include "iota/strings.hh"

// Debug
#include "debug/assert.hh"

// plus
#include "plus/concat.hh"

// Nitrogen
#include "Nitrogen/OSStatus.hh"

// Io: MacFiles
#include "MacFiles/Classic.hh"

// poseven
#include "poseven/functions/open.hh"
#include "poseven/functions/read.hh"
#include "poseven/functions/stat.hh"
#include "poseven/functions/write.hh"
#include "poseven/types/exit_t.hh"

// Divergence
#include "Divergence/Utilities.hh"

// Misc
#include "MacBinary.hh"

// Orion
#include "Orion/get_options.hh"
#include "Orion/Main.hh"


namespace tool
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	namespace Div = Divergence;
	namespace o = orion;
	
	
	// Operations:
	// 
	// Foo must be a real (possibly nonexisting) file (not a device stream)
	// 
	// --encode Foo foo.out   # If foo.out is omitted, a name is chosen.
	// --decode foo.in Foo    # If Foo is omitted, the MacBinary-specified name is used.
	// --check foo.in         # tests whether or not foo.in is a MacBinary envelope.
	
	
	static int Usage()
	{
		p7::write( p7::stderr_fileno, STR_LEN( "Usage: macbin --encode Source [dest.mbin]\n"
		                                       "       macbin --decode source.bin [dest-dir]\n" ) );
		
		return 1;
	}
	
	static void BlockWrite( int fd, const void* data, std::size_t byteCount )
	{
		p7::write( p7::fd_t( fd ), (const char*) data, byteCount );
	}
	
	static void Decode( p7::fd_t input, const N::FSDirSpec& destDir )
	{
		MacBinary::Decoder decoder( destDir );
		
		const std::size_t blockSize = 4096;
		
		char data[ blockSize ];
		
		std::size_t totalBytes = 0;
		
		try
		{
			while ( std::size_t bytes = p7::read( input, data, blockSize ) )
			{
				decoder.Write( data, bytes );
				
				totalBytes += bytes;
			}
		}
		catch ( const MacBinary::InvalidMacBinaryHeader& )
		{
			std::fprintf( stderr, "Invalid MacBinary header somewhere past offset %x\n", totalBytes );
			
			throw p7::exit_failure;
		}
	}
	
	static plus::string make_archive_name( const char* name )
	{
		size_t length = strlen( name );
		
		ASSERT( length > 0 );
		
		const char* extension = ".mBin";
		
		if ( name[ length - 1 ] == '/' )
		{
			--length;
		}
		else if ( p7::s_isreg( p7::stat( name ) ) )
		{
			extension = ".mbin";
		}
		
		return plus::concat( name, length, extension, STRLEN( ".mbin" ) );
	}
	
	int Main( int argc, char** argv )
	{
		const char* encode_target = NULL;
		const char* decode_target = NULL;
		
		o::bind_option_to_variable( "--encode", encode_target );
		o::bind_option_to_variable( "--decode", decode_target );
		
		o::get_options( argc, argv );
		
		char const *const *free_args = o::free_arguments();
		
		const size_t n_args = o::free_argument_count();
		
		if ( bool both_or_neither = (encode_target == NULL) == (decode_target == NULL) )
		{
			// There can be only one!
			return Usage();
		}
		
		if ( encode_target )
		{
			// FIXME:  Can't encode to a non-file stream, including stdout
			FSSpec targetFile = Div::ResolvePathToFSSpec( encode_target );
			
			if ( !io::item_exists( targetFile ) )
			{
				std::fprintf( stderr, "macbin: %s: %s\n", encode_target, std::strerror( ENOENT ) );
				
				return EXIT_FAILURE;
			}
			
			MacBinary::Encode( targetFile,
			                   &BlockWrite,
			                   p7::open( n_args > 0 ? free_args[ 0 ] : make_archive_name( encode_target ).c_str(),
			                             p7::o_wronly | p7::o_excl | p7::o_creat ) );
		}
		else if ( decode_target )
		{
			const char* destDirPath = n_args > 0 ? free_args[ 0 ] : ".";
			
			try
			{
				if ( bool use_stdin = decode_target[0] == '-'  &&  decode_target[1] == '\0' )
				{
					decode_target = "/dev/fd/0";
				}
				
				Decode( p7::open( decode_target, p7::o_rdonly ),
				        N::FSpMake_FSDirSpec( Div::ResolvePathToFSSpec( destDirPath ) ) );
			}
			catch ( const MacBinary::InvalidMacBinaryHeader& )
			{
				std::fprintf( stderr, "macbin: %s: invalid MacBinary header\n", decode_target );
				return 1;
			}
			catch ( const MacBinary::IncompatibleMacBinaryHeader& )
			{
				std::fprintf( stderr, "macbin: %s: incompatible (newer) MacBinary header\n", decode_target );
				return 2;
			}
		}
		else
		{
			return Usage();
		}
		
		return 0;
	}

}

