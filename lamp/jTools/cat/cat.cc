/*	======
 *	cat.cc
 *	======
 */

// Standard C/C++
#include <cerrno>

// Standard C/C++
#include <cstdio>

// POSIX
#include <fcntl.h>
#include <unistd.h>

// POSeven
#include "POSeven/Errno.hh"
#include "POSeven/FileDescriptor.hh"
#include "POSeven/Open.hh"

// Orion
#include "Orion/Main.hh"


namespace NN = Nucleus;
namespace p7 = poseven;
namespace O = Orion;


static bool PathnameMeansStdIn( const char* pathname )
{
	return    pathname[0] == '-'
	       && pathname[1] == '\0';
}

static const char* EvaluateMetaFilename( const char* pathname )
{
	if ( PathnameMeansStdIn( pathname ) )
	{
		return "/dev/fd/0";
	}
	
	return pathname;
}

static void DumpFile( p7::fd_t in )
{
	while ( true )
	{
		try
		{
			enum { blockSize = 4096 };
			
			char data[ blockSize ];
			
			int bytes = p7::read( in, data, blockSize );
			
			p7::write( p7::stdout_fileno, data, bytes );
		}
		catch ( const io::end_of_input& err )
		{
			break;
		}
		catch ( const io::no_input_pending& )
		{
			sleep( 0 );
		}
	}
}

int O::Main( int argc, argv_t argv )
{
	// Check for sufficient number of args
	if ( argc < 2 )
	{
		static const char* default_argv[] = { "cat", "-", NULL };
		
		argv = default_argv;
		
		argc = 2;
	}
	
	// Print each file in turn.  Return whether any errors occurred.
	int exit_status = EXIT_SUCCESS;
	
	for ( int index = 1;  index < argc;  ++index )
	{
		const char* pathname = EvaluateMetaFilename( argv[ index ] );
		
		try
		{
			NN::Owned< p7::fd_t > fd = p7::open( pathname, p7::o_rdonly );
			
			DumpFile( fd );
		}
		catch ( const p7::errno_t& error )
		{
			std::fprintf( stderr, "%s: %s: %s\n", argv[0], pathname, std::strerror( error ) );
			
			exit_status = EXIT_FAILURE;
			
			continue;
		}
	}
	
	return exit_status;
}

