/*
	macbin.cc
	---------
*/

// POSIX
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <stdio.h>
#include <string.h>

// Iota
#include "iota/strings.hh"

// mac-sys-utils
#include "mac_sys/errno_from_mac_error.hh"

// mac-file-utils
#include "mac_file/directory.hh"

// mac-relix-utils
#include "mac_relix/FSSpec_from_path.hh"

// Debug
#include "debug/assert.hh"

// plus
#include "plus/string/concat.hh"

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"

// poseven
#include "poseven/functions/open.hh"
#include "poseven/functions/read.hh"
#include "poseven/functions/stat.hh"
#include "poseven/functions/write.hh"
#include "poseven/types/exit_t.hh"

// Misc
#include "MacBinary.hh"

// Orion
#include "Orion/Main.hh"


namespace tool
{
	
	namespace p7 = poseven;
	
	using mac::sys::Error;
	using mac::sys::errno_from_mac_error;
	
	using mac::types::VRefNum_DirID;
	
	
	// Operations:
	// 
	// Foo must be a real (possibly nonexisting) file (not a device stream)
	// 
	// --encode Foo foo.out   # If foo.out is omitted, a name is chosen.
	// --decode foo.in Foo    # If Foo is omitted, the MacBinary-specified name is used.
	// --check foo.in         # tests whether or not foo.in is a MacBinary envelope.
	
	
	static int Usage()
	{
		p7::write( p7::stderr_fileno, STR_LEN( "Usage: macbin encode Source [dest.mbin]\n"
		                                       "       macbin decode source.bin [dest-dir]\n" ) );
		
		return 1;
	}
	
	static void BlockWrite( int fd, const void* data, size_t byteCount )
	{
		p7::write( p7::fd_t( fd ), (const char*) data, byteCount );
	}
	
	static
	void Decode( p7::fd_t input, const VRefNum_DirID& destDir )
	{
		MacBinary::Decoder decoder( destDir );
		
		const size_t blockSize = 4096;
		
		char data[ blockSize ];
		
		size_t totalBytes = 0;
		
		try
		{
			while ( size_t bytes = p7::read( input, data, blockSize ) )
			{
				decoder.Write( data, bytes );
				
				totalBytes += bytes;
			}
		}
		catch ( const MacBinary::InvalidMacBinaryHeader& )
		{
			fprintf( stderr, "Invalid MacBinary header somewhere past offset %lx\n", totalBytes );
			
			throw p7::exit_failure;
		}
	}
	
	static plus::string make_archive_name( const char* name )
	{
		size_t length = strlen( name );
		
		ASSERT( length > 0 );
		
		const char* extension = ".mbim";  // Default MacBinary+ extension
		
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
		if ( argc < 1 + 2 )
		{
			return Usage();
		}
		
		const char* command = argv[ 1 ];
		const char* target  = argv[ 2 ];
		const char* dest    = argv[ 3 ];
		
		if ( command[ 0 ] == '-'  &&  command[ 1 ] == '-' )
		{
			command += 2;
		}
		
		const char* encode_target = NULL;
		const char* decode_target = NULL;
		
		if ( strcmp( command, "encode" ) == 0 )
		{
			encode_target = target;
		}
		else if ( strcmp( command, "decode" ) == 0 )
		{
			decode_target = target;
		}
		else
		{
			return Usage();
		}
		
		using mac::relix::FSSpec_from_existing_path;
		
		if ( encode_target )
		{
			// FIXME:  Can't encode to a non-file stream, including stdout
			
			FSSpec targetFile;
			
			Error err = FSSpec_from_existing_path( encode_target, targetFile );
			
			int errnum = is_errno( err ) ? errno_from_muxed( err )
			                             : errno_from_mac_error( err );
			
			if ( errnum > 0 )
			{
				fprintf( stderr, "macbin: %s: %s\n", encode_target, strerror( errnum ) );
				
				return 1;
			}
			
			Mac::ThrowOSStatus( err );
			
			MacBinary::Encode( targetFile,
			                   &BlockWrite,
			                   p7::open( dest ? dest : make_archive_name( encode_target ).c_str(),
			                             p7::o_wronly | p7::o_excl | p7::o_creat ) );
		}
		else if ( decode_target )
		{
			const char* destDirPath = dest ? dest : ".";
			
			FSSpec dest;
			
			Error err = FSSpec_from_existing_path( destDirPath, dest );
			
			int errnum = is_errno( err ) ? errno_from_muxed( err )
			                             : errno_from_mac_error( err );
			
			if ( errnum > 0 )
			{
				fprintf( stderr, "macbin: %s: %s\n", destDirPath, strerror( errnum ) );
				
				return 1;
			}
			
			Mac::ThrowOSStatus( err );
			
			try
			{
				if ( bool use_stdin = decode_target[0] == '-'  &&  decode_target[1] == '\0' )
				{
					decode_target = "/dev/fd/0";
				}
				
				VRefNum_DirID destDir = mac::file::directory( dest );
				
				Decode( p7::open( decode_target, p7::o_rdonly ), destDir );
			}
			catch ( const MacBinary::IncompatibleMacBinaryHeader& )
			{
				fprintf( stderr, "macbin: %s: incompatible (newer) MacBinary header\n", decode_target );
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
