/*
	ar.cc
	-----
*/

// Standard C++
#include <list>

// Standard C/C++
#include <cstring>

// Iota
#include "iota/strings.hh"

// plus
#include "plus/var_string.hh"

// vxo
#include "vxo/ptrvec.hh"

// poseven
#include "poseven/functions/execvp.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/pread.hh"
#include "poseven/functions/read.hh"
#include "poseven/functions/vfork.hh"
#include "poseven/functions/write.hh"
#include "poseven/functions/wait.hh"

// mac_pathname
#include "mac_pathname_from_path.hh"

// Orion
#include "Orion/Main.hh"

// metrowerks
#include "metrowerks/object_file.hh"


namespace tool
{
	
	namespace n = nucleus;
	namespace p7 = poseven;
	namespace mw = metrowerks;
	
	
	template < class Iter >
	plus::string join( const plus::string& glue, Iter begin, Iter end )
	{
		if ( begin == end )
		{
			return "";
		}
		
		plus::var_string result = *begin++;
		
		while ( begin != end )
		{
			result += glue;
			result += *begin++;
		}
		
		return result;
	}
	
	
	static const char* StoreMacPathFromPOSIXPath( const char* pathname )
	{
		static std::list< plus::string > static_string_storage;
		
		static_string_storage.push_back( mac_pathname_from_path( pathname, true ) );
		
		return static_string_storage.back().c_str();
	}
	
	
	static bool HasFullPathname( p7::fd_t object_file_stream )
	{
		char buffer[ 512 ];
		
		ssize_t bytes_read = p7::read( object_file_stream, buffer, sizeof buffer );
		
		const char* buffer_end = buffer + sizeof buffer;
		
		for ( const char* p = buffer;  p != buffer_end;  ++p )
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
	
	static bool HasSymH( p7::fd_t                       object_file_stream,
	                     const mw::object_file_header&  file_header )
	{
		mw::object_header object_header;
		
		const ssize_t bytes_read = p7::pread( object_file_stream,
		                                      (char*) &object_header,
		                                      sizeof object_header,
		                                      file_header.object_offset );
		
		if ( bytes_read != sizeof object_header )
		{
			// complain
		}
		
		return object_header.sym_offset != 0;
	}
	
	static void CheckObjectFile( p7::fd_t               object_file_stream,
	                             mw::cpu_architecture&  arch,
	                             bool&                  debug )
	{
		mw::object_file_header file_header;
		
		ssize_t bytes_read = p7::read( object_file_stream, (char*) &file_header, sizeof file_header );
		
		if ( bytes_read != sizeof file_header )
		{
			// complain
			return;
		}
		
		if ( file_header.magic_number != mw::object_file_magic )
		{
			// complain
			return;
		}
		
		switch ( arch = file_header.cpu_arch )
		{
			case mw::cpu_m68k:
				debug = HasFullPathname( object_file_stream );
				break;
			
			case mw::cpu_powerpc:
				debug = HasSymH( object_file_stream, file_header );
				break;
			
			default:
				// complain
				break;
		}
	}
	
	static void CheckObjectFile( const char*            object_path,
	                             mw::cpu_architecture&  arch,
	                             bool&                  debug )
	{
		CheckObjectFile( p7::open( object_path, p7::o_rdonly ), arch, debug );
	}
	
	
	int Main( int argc, char** argv )
	{
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
		
		plus::string output_mac_pathname = mac_pathname_from_path( output_path, true );
		
		mw::cpu_architecture arch = mw::cpu_unknown;
		
		bool debug = false;
		
		CheckObjectFile( first_input_path, arch, debug );
		
		vxo::PtrVec_< const char > command;
		
		command.push_back( "tlsrvr"   );
		command.push_back( "--escape" );  // escape arguments to prevent expansion
		command.push_back( "--"       );  // stop interpreting options here
		
		switch ( arch )
		{
			case mw::cpu_m68k:
				command.push_back( "MWLink68K" );
				command.push_back( "-model"    );
				command.push_back( "far"       );
				break;
			
			case mw::cpu_powerpc:
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
			
			if ( arch == mw::cpu_powerpc )
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
			plus::var_string output = join( " ", command.begin(), command.end() );
			
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
