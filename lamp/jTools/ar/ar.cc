/*	=====
 *	ar.cc
 *	=====
 */

// Standard C++
#include <string>

// Standard C/C++
#include <cstring>

// Iota
#include "iota/strings.hh"

// poseven
#include "poseven/functions/execvp.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/pread.hh"
#include "poseven/functions/read.hh"
#include "poseven/functions/vfork.hh"
#include "poseven/functions/write.hh"
#include "poseven/functions/wait.hh"

// Nitrogen
#include "Nitrogen/OSStatus.h"

// GetPathname
#include "GetPathname.hh"

// Divergence
#include "Divergence/Utilities.hh"

// Orion
#include "Orion/Main.hh"


namespace tool
{
	
	namespace N = Nitrogen;
	namespace n = nucleus;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	namespace Div = Divergence;
	
	
	template < class Iter >
	std::string join( const std::string& glue, Iter begin, Iter end )
	{
		if ( begin == end )
		{
			return "";
		}
		
		std::string result = *begin++;
		
		while ( begin != end )
		{
			result += glue;
			result += *begin++;
		}
		
		return result;
	}
	
	
	static std::string MacPathFromPOSIXPath( const char* pathname )
	{
		FSSpec item = Div::ResolvePathToFSSpec( pathname );
		
		return GetMacPathname( item );
	}
	
	static const char* StoreMacPathFromPOSIXPath( const char* pathname )
	{
		static std::list< std::string > static_string_storage;
		
		static_string_storage.push_back( MacPathFromPOSIXPath( pathname ) );
		
		return static_string_storage.back().c_str();
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
		ObjectHeader object_header;
		
		const ssize_t bytes_read = p7::pread( object_file_stream,
		                                      (char*) &object_header,
		                                      sizeof object_header,
		                                      file_header.objectOffset );
		
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
	
	
	int Main( int argc, iota::argv_t argv )
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
		
		std::vector< const char* > command;
		
		command.push_back( "tlsrvr"   );
		command.push_back( "--switch" );  // bring ToolServer to front
		command.push_back( "--escape" );  // escape arguments to prevent expansion
		command.push_back( "--"       );  // stop interpreting options here
		
		std::string linker = "MWLinkUnsupportedArchitecture";
		
		switch ( arch )
		{
			case archM68K:
				command.push_back( "MWLink68K" );
				command.push_back( "-model"    );
				command.push_back( "far"       );
				break;
			
			case archPPC:
				command.push_back( "MWLinkPPC" );
				break;
			
			default:
				// complain
				break;
		};
		
		command.push_back( "-xm" );
		command.push_back( "l"   );
		
		if ( debug )
		{
			command.push_back( "-sym" );
			command.push_back( "full" );
			
			if ( arch == archPPC )
			{
				command.push_back( "-tb" );
				command.push_back( "on"  );
			}
		}
		
		command.push_back( "-o" );
		command.push_back( output_mac_pathname.c_str() );
		
		bool dry_run = false;
		bool verbose = false;
		
		argv += 2;  // skip rcs and output.lib
		
		while ( const char* arg = *++argv )
		{
			if ( arg[0] == '-' )
			{
				switch ( arg[1] )
				{
					case 'n':
						dry_run = true;
						break;
					
					case 'v':
						verbose = true;
						break;
					
					default:
						break;
				}
			}
			else
			{
				// translate path
				command.push_back( StoreMacPathFromPOSIXPath( arg ) );
			}
		}
		
		if ( verbose )
		{
			std::string output = join( " ", command.begin(), command.end() );
			
			output += '\n';
			
			p7::write( p7::stdout_fileno, output );
		}
		
		command.push_back( NULL );
		
		if ( dry_run )
		{
			return p7::exit_success;
		}
		
		p7::pid_t pid = POSEVEN_VFORK();
		
		if ( pid == 0 )
		{
			p7::execvp( &command[0] );
		}
		
		return n::convert< p7::exit_t >( p7::wait() );
	}
	
}

