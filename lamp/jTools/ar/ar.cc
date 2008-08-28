/*	=====
 *	ar.cc
 *	=====
 */

// Standard C++
#include <string>

// Standard C/C++
#include <cstring>

// Standard C
#include <stdlib.h>

// POSIX
#include <sys/wait.h>

// Iota
#include "iota/strings.hh"

// POSeven
#include "POSeven/Open.hh"
#include "POSeven/Pathnames.hh"
#include "POSeven/Stat.hh"

// Nitrogen
#include "Nitrogen/OSStatus.h"

// GetPathname
#include "GetPathname.hh"

// Divergence
#include "Divergence/Utilities.hh"

// Orion
#include "Orion/GetOptions.hh"
#include "Orion/Main.hh"


namespace O = Orion;


static int exit_from_wait( int stat )
{
	int result = WIFEXITED( stat )   ? WEXITSTATUS( stat )
	           : WIFSIGNALED( stat ) ? WTERMSIG( stat ) + 128
	           :                       -1;
	
	return result;
}


namespace jTools
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	namespace Div = Divergence;
	
	using namespace io::path_descent_operators;
	
	
	static std::string MacPathFromPOSIXPath( const char* pathname )
	{
		FSSpec item = Div::ResolvePathToFSSpec( pathname );
		
		return GetMacPathname( item );
	}
	
	static std::string QuotedMacPathFromPOSIXPath( const char* pathname )
	{
		return "'" + MacPathFromPOSIXPath( pathname ) + "'";
	}
	
	static std::string OutputMacPathname( const char* output_path )
	{
		return GetMacPathname( Div::ResolvePathToFSSpec( output_path ) );
	}
	
	
	enum Architecture
	{
		archUnknown = 0,
		
		archM68K = 'M68K',
		archPPC  = 'PPC '
	};
	
	struct MetrowerksObjectFileHeader
	{
		::OSType magicNumber;
		
		Architecture architecture;
		
		UInt32 _8, _12, _16, _20, _24, _28, _32, _36;
		
		UInt32 objectOffset;
		UInt32 objectLength;
	};
	
	struct ObjectHeader
	{
		::OSType magicNumber;
		
		UInt32 _4, _8, _12, _16;
		
		UInt32 symOffset;
		UInt32 symLength;
	};
	
	static bool HasFullPathname( p7::fd_t object_file_stream )
	{
		char buffer[ 512 ];
		
		ssize_t bytes_read = p7::read( object_file_stream, buffer, sizeof buffer );
		
		const char* buffer_end = buffer + sizeof buffer;
		
		const char* p = buffer;
		
		for ( p;  p != buffer_end;  ++p )
		{
			if ( *p == ':' )
			{
				return true;
			}
			
			if ( *p == '\0' )
			{
				break;
			}
		}
		
		return false;
	}
	
	static bool HasSymH( p7::fd_t object_file_stream, const MetrowerksObjectFileHeader& file_header )
	{
		p7::throw_posix_result( lseek( object_file_stream, file_header.objectOffset, SEEK_SET ) );
		
		ObjectHeader object_header;
		
		ssize_t bytes_read = p7::read( object_file_stream, (char*) &object_header, sizeof object_header );
		
		if ( bytes_read != sizeof object_header )
		{
			// complain
		}
		
		return object_header.symOffset != 0;
	}
	
	static void CheckObjectFile( p7::fd_t       object_file_stream,
	                             Architecture&  arch,
	                             bool&          debug )
	{
		MetrowerksObjectFileHeader file_header;
		
		ssize_t bytes_read = p7::read( object_file_stream, (char*) &file_header, sizeof file_header );
		
		if ( bytes_read != sizeof file_header )
		{
			// complain
			return;
		}
		
		if ( file_header.magicNumber != 'MWOB' )
		{
			// complain
			return;
		}
		
		switch ( arch = file_header.architecture )
		{
			case archM68K:
				debug = HasFullPathname( object_file_stream );
				break;
			
			case archPPC:
				debug = HasSymH( object_file_stream, file_header );
				break;
			
			default:
				// complain
				break;
		}
	}
	
	static void CheckObjectFile( const char*    object_path,
	                             Architecture&  arch,
	                             bool&          debug )
	{
		CheckObjectFile( io::open_for_reading( object_path ), arch, debug );
	}
	
	
	static int Main( int argc, iota::argv_t argv )
	{
		NN::RegisterExceptionConversion< NN::Exception, N::OSStatus >();
		
		// $ ar rcs output.lib input{1,2}.o
		
		if ( argc < 4 )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "ar: insufficient arguments\nUsage: ar rcs out.lib in1.o ...\n" ) );
			
			return 2;
		}
		
		if ( std::strcmp( argv[1], "rcs" ) != 0 )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "ar: arg 1 must be 'rcs'\nUsage: ar rcs out.lib in1.o ...\n" ) );
			
			return 2;
		}
		
		std::size_t n_inputs = argc - 3;
		
		const char* output_path = argv[ 2 ];
		
		const char* first_input_path = argv[ 3 ];
		
		std::string output_mac_pathname = GetMacPathname( Div::ResolvePathToFSSpec( output_path ) );
		
		Architecture arch = archUnknown;
		
		bool debug = false;
		
		CheckObjectFile( first_input_path, arch, debug );
		
		std::string linker = "MWLinkUnsupportedArchitecture";
		
		switch ( arch )
		{
			case archM68K:
				linker = "MWLink68K -model far";
				break;
			
			case archPPC:
				linker = "MWLinkPPC";
				break;
			
			default:
				// complain
				break;
		};
		
		std::string product = "-xm l";
		std::string ldArgs;
		
		bool verbose = false;
		
		argv += 2;  // skip rcs and output.lib
		
		while ( const char* arg = *++argv )
		{
			if ( arg[0] == '-' )
			{
				switch ( arg[1] )
				{
					case 'v':
						verbose = true;
						continue;
					
					default:
						break;
				}
			}
			else
			{
				// translate path
				std::string mac_pathname = QuotedMacPathFromPOSIXPath( arg );
				
				ldArgs += ' ';
				ldArgs += mac_pathname;
			}
		}
		
		std::string debugging;
		
		if ( debug )
		{
			debugging = " -sym full";
			
			if ( arch == archPPC )
			{
				debugging += " -tb on";
			}
		}
		
		std::string output = " -o '" + output_mac_pathname + "'";
		
		ldArgs = " " + product + debugging + output + " " + ldArgs;
		
		std::string command = "tlsrvr --switch --escape -- " + linker + ldArgs + '\n';
		
		if ( verbose )
		{
			write( STDOUT_FILENO, command.data(), command.size() );
		}
		
		int wait_status = system( command.c_str() );
		
		int exit_status = exit_from_wait( wait_status );
		
		return exit_status;
	}
	
}

int O::Main( int argc, argv_t argv )
{
	return jTools::Main( argc, argv );
}

