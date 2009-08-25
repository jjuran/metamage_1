/*	=======
 *	mwcc.cc
 *	=======
 */

// Standard C++
#include <vector>

// POSeven
#include "POSeven/functions/execvp.hh"
#include "POSeven/functions/stat.hh"
#include "POSeven/functions/vfork.hh"
#include "POSeven/functions/wait.hh"
#include "POSeven/functions/write.hh"
#include "POSeven/Pathnames.hh"

// Nitrogen
#include "Nitrogen/MacErrors.h"
#include "Nitrogen/Str.h"

// GetPathname
#include "GetPathname.hh"

// Divergence
#include "Divergence/Utilities.hh"

// Orion
#include "Orion/Main.hh"


namespace tool
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	namespace Div = Divergence;
	
	
	using namespace io::path_descent_operators;
	
	
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
	
	
	enum Architecture
	{
		arch_none,
		arch_m68k,
		arch_ppc
	};
	
	static Architecture read_arch( const char* arch )
	{
		if ( std::strcmp( arch, "m68k" ) == 0 )
		{
			return arch_m68k;
		}
		
		if ( std::strcmp( arch, "ppc" ) == 0 )
		{
			return arch_ppc;
		}
		
		return arch_none;
	}
	
	static bool rtti = true;
	static bool cfm = false;
	static bool a4 = false;
	static bool traceback = false;
	
	static void set_codegen_flag( const std::string& flag )
	{
		if ( flag == "-fpascal-strings" )
		{
			// Mac compilers use Pascal strings by default
		}
		else if ( flag == "-fno-rtti" )
		{
			rtti = false;
		}
		else if ( flag == "-mCFM" )
		{
			cfm = true;
		}
		else if ( flag == "-mA4-globals" )
		{
			a4 = true;
		}
		else if ( flag == "-mtraceback" )
		{
			traceback = true;
		}
	}
	
	static std::string MacPathFromPOSIXPath( const char* pathname )
	{
		FSSpec item = Div::ResolvePathToFSSpec( pathname );
		
		return GetMacPathname( item );
	}
	
	static const std::string& MacPathForCWD()
	{
		static std::string mac_path = MacPathFromPOSIXPath( "." );
		
		return mac_path;
	}
	
	static std::string ShortMacPathFromPOSIXPath( const char* pathname )
	{
		std::string mac_path = MacPathFromPOSIXPath( pathname );
		
		const std::string& mac_cwd = MacPathForCWD();
		
		const bool within =  mac_path.length() > mac_cwd.length()
		                  && std::equal( mac_cwd.begin(),
		                                 mac_cwd.end(),
		                                 mac_path.begin() );
		
		if ( within )
		{
			const bool trailing_colon = (*mac_cwd.rbegin() == ':');
			
			const std::size_t common = mac_cwd.length() - trailing_colon;
			
			mac_path.erase( 0, common );
		}
		
		return mac_path;
	}
	
	static bool extension_begins_with_char( const char* path, char c )
	{
		const char* dot   = std::strrchr( path, '.' );
		const char* slash = std::strrchr( path, '/' );
		
		const bool has_dot = dot > slash;  // Works even if slash or dot is NULL
		
		return has_dot  &&  dot[ 1 ] == c;
	}
	
	inline bool is_object_filename( const char* path )
	{
		return extension_begins_with_char( path, 'o' );
	}
	
	std::vector< const char* > gIncludeDirs;
	
	static void RememberIncludeDir( const char* pathname )
	{
		gIncludeDirs.push_back( pathname );
	}
	
	static std::string get_prefix_image_path( const char* prefix_path )
	{
		const bool header_is_source = extension_begins_with_char( prefix_path, 'h' );
		
		if ( !header_is_source )
		{
			return prefix_path;
		}
		
		std::string prefix_image_path = prefix_path;
		
		prefix_image_path += ".mwch";
		
		const bool filename_only = std::strchr( prefix_path, '/' ) == NULL;
		
		if ( !filename_only )
		{
			return prefix_image_path;
		}
		
		typedef std::vector< const char* >::const_iterator Iter;
		
		for ( Iter it = gIncludeDirs.begin();  it != gIncludeDirs.end();  ++it )
		{
			std::string location = *it / prefix_image_path;
			
			if ( io::file_exists( location ) )
			{
				return location;
			}
		}
		
		// FIXME:  Should probably throw ENOENT
		return prefix_image_path;
	}
	
	static const char* store_string( const std::string& string )
	{
		static std::list< std::string > static_string_storage;
		
		static_string_storage.push_back( string );
		
		return static_string_storage.back().c_str();
	}
	
	static const char* store_mac_path_from_posix_path( const char* path )
	{
		return store_string( ShortMacPathFromPOSIXPath( path ) );
	}
	
	
	int Main( int argc, iota::argv_t argv )
	{
		NN::RegisterExceptionConversion< NN::Exception, N::OSStatus >();
		
		std::vector< const char* > command_args;
		
		Architecture arch = TARGET_CPU_68K ? arch_m68k
		                  : TARGET_CPU_PPC ? arch_ppc
		                  :                  arch_none;
		
		const char* output_pathname = NULL;
		
		const char* opt = NULL;
		
		bool debug   = false;
		bool dry_run = false;
		bool verbose = false;
		
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
					
					case 'c':
						// 'gcc -c' means compile
						break;
					
					case 'W':
						// ignore gcc warning options
						break;
					
					case 'D':
						command_args.push_back( arg );
						break;
					
					case 'a':
						if ( std::strcmp( arg + 1, "arch" ) == 0 )
						{
							arch = read_arch( *++argv );
						}
						break;
					
					case 'm':
					case 'f':
						set_codegen_flag( arg );
						break;
					
					case 'g':
						debug = true;
						break;
					
					case 'O':
						opt = arg;
						break;
					
					case 'o':
						if ( arg[2] == '\0' )
						{
							output_pathname = *++argv;
						}
						break;
					
					case 'I':
						RememberIncludeDir( arg + 2 );
						
						command_args.push_back( store_string( "-I" + MacPathFromPOSIXPath( arg + 2 ) ) );
						break;
					
					case 'i':
						if ( std::strcmp( arg + 1, "include" ) == 0 )
						{
							std::string prefix_path = get_prefix_image_path( *++argv );
							
							command_args.push_back( "-prefix" );
							
							command_args.push_back( store_mac_path_from_posix_path( prefix_path.c_str() ) );
						}
						break;
					
					default:
						break;
				}
			}
			else if ( std::strchr( arg, '/' ) )
			{
				command_args.push_back( store_mac_path_from_posix_path( arg ) );
			}
		}
		
		if ( output_pathname == NULL )
		{
			std::fprintf( stderr, "%s\n", "mwcc: -o is required" );
			
			return EXIT_FAILURE;
		}
		
		std::vector< const char* > command;
		
		command.push_back( "tlsrvr"   );
		command.push_back( "--switch" );  // bring ToolServer to front
		command.push_back( "--escape" );  // escape arguments to prevent expansion
		command.push_back( "--"       );  // stop interpreting options here
		
		switch ( arch )
		{
			default:
			case arch_none:
				std::fprintf( stderr, "%s\n", "mwcc: invalid architecture" );
				
				return EXIT_FAILURE;
			
			case arch_m68k:
				command.push_back( "MWC68K"   );
				command.push_back( "-mc68020" );
				command.push_back( "-model"   );
				command.push_back( "far"      );
				
				if ( a4 )
				{
					command.push_back( "-a4" );
				}
				else if ( cfm )
				{
					command.push_back( "-model"     );
					command.push_back( "-CFMflatdf" );
				}
				break;
			
			case arch_ppc:
				command.push_back( "MWCPPC" );
				break;
		}
		
		command.push_back( is_object_filename( output_pathname ) ? "-o" : "-precompile" );
		
		command.push_back( store_mac_path_from_posix_path( output_pathname ) );
		
		if ( !rtti )
		{
			command.push_back( "-RTTI" );
			command.push_back( "off"   );
		}
		
		if ( debug )
		{
			command.push_back( "-sym" );
			command.push_back( "full" );
		}
		
		if ( arch == arch_m68k  &&  !debug )
		{
			command.push_back( "-mbg" );
			command.push_back( "off"  );
		}
		
		if ( arch == arch_ppc  &&  debug  ||  traceback )
		{
			command.push_back( "-tb" );
			command.push_back( "on"  );
		}
		
		command.push_back( "-opt" );
		command.push_back( opt[2] == '0' ? "off" : "full" );
		
		command.push_back( "-nosyspath"      );
		command.push_back( "-convertpaths"   );
		command.push_back( "-nomapcr"        );
		command.push_back( "-once"           );
		command.push_back( "-DPRAGMA_ONCE=1" );
		
		command.push_back( "-D" "__LAMP__=1" );
		
		// Assume CW Pro 6
		command.push_back( "-D" "NO_POINTER_TO_MEMBER_TEMPLATE_PARAMETERS=1" );
		
		command.push_back( "-w"                                                   );
		command.push_back( "all,nounusedarg,noimplicit,nonotinlined,noextracomma" );
		
		command.push_back( "-ext" );
		command.push_back( "o"    );
		
		command.push_back( "-maxerrors" );
		command.push_back( "8"          );
		
		command.push_back( "-proto" );
		command.push_back( "strict" );
		
		command.insert( command.end(), command_args.begin(), command_args.end() );
		
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
		
		return NN::Convert< p7::exit_t >( p7::wait() );
	}
	
}

