/*
	ld.cc
	-----
*/

// Standard C++
#include <list>

// Standard C/C++
#include <cstdio>

// Standard C
#include <stdlib.h>
#include <string.h>

// iota
#include "iota/strings.hh"

// gear
#include "gear/quad.hh"

// plus
#include "plus/mac_utf8.hh"
#include "plus/string/concat.hh"

// vxo
#include "vxo/ptrvec.hh"

// poseven
#include "poseven/extras/slurp.hh"
#include "poseven/functions/basename.hh"
#include "poseven/functions/execvp.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/read.hh"
#include "poseven/functions/vfork.hh"
#include "poseven/functions/waitpid.hh"
#include "poseven/types/exit_t.hh"
#include "poseven/types/fd_t.hh"

// pfiles
#include "pfiles/common.hh"

// mac_pathname
#include "mac_pathname_from_path.hh"

// Orion
#include "Orion/Main.hh"

// metrowerks
#include "metrowerks/object_file.hh"

// one_path
#include "one_path/find_InterfacesAndLibraries.hh"


namespace tool
{
	
	namespace p7 = poseven;
	namespace mw = metrowerks;
	
	
	using namespace io::path_descent_operators;
	
	
	enum MacAPI
	{
		kMacAPINone,
		kMacAPIBlue,
		kMacAPICarbon
	};
	
	const char* gMacAPINames[] =
	{
		"(None)",
		"(Blue)",
		"Carbon"
	};
	
	static MacAPI gMacAPI = kMacAPINone;
	
	static bool gCFM68K = false;
	
	static const char* gFirstObjectFilePath = NULL;
	
	
	enum Architecture
	{
		arch_none,
		arch_m68k,
		arch_ppc,
		
	#if __MC68K__
		
		arch_default = arch_m68k
		
	#elif __POWERPC__
		
		arch_default = arch_ppc
		
	#else
		
		arch_default = arch_none
		
	#endif
	};
	
	static Architecture arch = arch_default;
	
	static const char* output_pathname = NULL;
	
	static bool sym     = true;
	static bool debug   = true;
	static bool dry_run = false;
	static bool verbose = false;
	
	
	template < class Iter >
	plus::string join( Iter begin, Iter end, const plus::string& glue = "" )
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
	
	
	static const plus::string& get_Libraries_pathname()
	{
		static plus::string libraries = find_InterfacesAndLibraries() + "/Libraries/";
		
		return libraries;
	}
	
	static plus::string get_Libraries_subdir( const char* name )
	{
		if ( const char* path = getenv( name ) )
		{
			return path;
		}
		
		return get_Libraries_pathname() + name;
	}
	
	static plus::string get_PPCLibraries()
	{
		return get_Libraries_subdir( "PPCLibraries" );
	}
	
	static plus::string get_SharedLibraries()
	{
		return get_Libraries_subdir( "SharedLibraries" );
	}
	
	static plus::string FindSystemLibrary( const plus::string& libName )
	{
		char const* const MWLibs_name = arch == arch_m68k ? "MW68KLibraries"
		                              : arch == arch_ppc  ? "MWPPCLibraries"
		                              :                     "NoArchitecture";
		
		const plus::string a = get_Libraries_subdir( MWLibs_name ) / libName;
		
		if ( io::file_exists( a ) )
		{
			return a;
		}
		
		if ( arch == arch_ppc )
		{
			const plus::string b = get_PPCLibraries() / libName;
			
			if ( io::file_exists( b ) )
			{
				return b;
			}
		}
		
		if ( arch == arch_ppc  ||  gCFM68K )
		{
			const plus::string c = get_SharedLibraries() / libName;
			
			if ( io::file_exists( c ) )
			{
				return c;
			}
		}
		
		std::fprintf( stderr, "System library missing: %s\n", libName.c_str() );
		
		throw p7::exit_failure;
	}
	
	
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
	
	vxo::PtrVec_< const char > gLibraryDirs;
	
	static void RememberLibraryDir( const char* pathname )
	{
		gLibraryDirs.push_back( pathname );
	}
	
	static plus::string FindLibrary( const char* lib )
	{
		typedef vxo::PtrVec_< const char >::const_iterator Iter;
		
		plus::string filename = plus::concat( lib, STR_LEN( ".lib" ) );
		
		for ( Iter it = gLibraryDirs.begin();  it != gLibraryDirs.end();  ++it )
		{
			plus::string pathname = plus::string( *it ) / filename;
			
			if ( io::file_exists( pathname ) )
			{
				return pathname;
			}
		}
		
		std::fprintf( stderr, "ld: can't find library: %s\n", lib );
		
		throw p7::exit_failure;
	}
	
	enum ProductType
	{
		kProductTool,
		kProductCodeResource,
		kProductSharedLib,
		kProductApp
	};
	
	static ProductType gProductType = kProductTool;
	
	static const char* gFileType    = NULL;
	static const char* gFileCreator = NULL;
	
	static const char* store_string( const plus::string& string )
	{
		static std::list< plus::string > static_string_storage;
		
		static_string_storage.push_back( string );
		
		return static_string_storage.back().c_str();
	}
	
	static const char* StoreMacPathFromPOSIXPath( const char* pathname )
	{
		return store_string( mac_pathname_from_path( pathname, true ) );
	}
	
	
	static void check_object_file( p7::fd_t object_file_stream )
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
		
		if ( file_header.cpu_arch != mw::cpu_m68k )
		{
			// complain
			return;
		}
		
		char buffer[ 512 ];
		
		bytes_read = p7::read( object_file_stream, buffer, sizeof buffer );
		
		const char *const buffer_begin = buffer;  // appease picky compilers
		const char *const buffer_end   = buffer + sizeof buffer;
		
		const char* p = std::find( buffer_begin, buffer_end, '\0' );
		
		p += 4 - ((int) p & 0x3);  // skip padding
		
		if ( p + sizeof (mw::runtime_block) <= buffer_end )
		{
			const mw::runtime_block& rt = *(const mw::runtime_block*) p;
			
			gCFM68K = rt.runtime_arch == mw::runtime_cfm68k;
		}
		else
		{
			// Highly unlikely that a pathname would be this long, but if it
			// happens, let me know.
			p7::throw_errno( ENAMETOOLONG );
		}
	}
	
	static void check_object_file( const char* path )
	{
		check_object_file( p7::open( path, p7::o_rdonly ) );
	}
	
	
	static void do_hyphen_option( char**& argv, vxo::PtrVec_< const char >& command_args )
	{
		const char* arg = argv[0];
		
		switch ( arg[ 1 ] )
		{
			case 'n':
				dry_run = true;
				break;
			
			case 'v':
				verbose = true;
				break;
			
			case 'a':
				if ( std::strcmp( arg + 1, "arch" ) == 0 )
				{
					arch = read_arch( *++argv );
				}
				
				break;
			
			case 'd':
				if ( std::strcmp( arg + 1, "dynamic" ) == 0 )
				{
					gProductType = kProductSharedLib;
				}
				
				break;
			
			case 'S':
				if ( arg[2] == '\0' )
				{
					sym = false;
				}
				
				break;
			
			case 's':
				if ( arg[2] == '\0' )
				{
					sym = false;
					debug = false;
				}
				
				break;
			
			case 'w':
				if ( arg[2] == 'i'  &&  arg[3] == '\0' )
				{
					command_args.push_back( arg );
				}
				
				break;
			
			case 'r':
				if ( arg[2] == 't'  &&  arg[3] == '\0' )
				{
					command_args.push_back(    arg  );
					command_args.push_back( *++argv );
				}
				
				break;
			
			case 'o':
				if ( arg[2] == '\0' )
				{
					output_pathname = *++argv;
				}
				else if ( std::strcmp( arg + 1, "object" ) == 0 )
				{
					gProductType = kProductCodeResource;
				}
				
				break;
			
			case 'l':
				// new block
				{
					const char* lib_name = arg + 2;  // skip "-l"
					
					plus::string library_pathname = FindLibrary( lib_name );
					
					const char* mac_pathname = StoreMacPathFromPOSIXPath( library_pathname.c_str() );
					
					// Link __tool first, if present.
					// This hack is necessary on 68K to ensure that
					// _relix_main() resides within the first 32K,
					// accessible by JMP or JSR from the startup code.
					
					const bool expedited = strcmp( lib_name, "__tool" ) == 0;
					
					command_args.insert( ( expedited ? command_args.begin()
					                                 : command_args.end() ),
					                       mac_pathname );
				}
				
				break;
			
			case 'L':
				RememberLibraryDir( arg + 2 );
				break;
			
			default:
				break;
		}
	}
	
	static void do_plus_option( const char* arg, vxo::PtrVec_< const char >& command_args )
	{
		if ( const char* equals = std::strchr( arg, '=' ) )
		{
			plus::var_string option( arg, equals );
			
			option[0] = '-';
			
			command_args.push_back( store_string( option ) );
			
			command_args.push_back( equals + 1 );
		}
		else
		{
			// error
		}
	}
	
	static bool do_special_case_arg( const char* arg )
	{
		plus::string filename = p7::basename( arg );
		
		if ( filename == "CarbonLib" )
		{
			gMacAPI = kMacAPICarbon;
		}
		else if ( filename == "InterfaceLib" )
		{
			gMacAPI = kMacAPIBlue;
		}
		else if ( filename == "PkgInfo" )
		{
			plus::string pkgInfo = p7::slurp( arg );
			
			if ( pkgInfo.length() < sizeof 'Type' + sizeof 'Crtr' )
			{
				std::fprintf( stderr, "%s\n", "ld: PkgInfo is shorter than 8 bytes" );
				
				throw p7::exit_failure;
			}
			
			plus::string type   ( pkgInfo.data(),     4 );
			plus::string creator( pkgInfo.data() + 4, 4 );
			
			gFileType    = store_string( plus::utf8_from_mac( type    ) );
			gFileCreator = store_string( plus::utf8_from_mac( creator ) );
			
			const uint32_t typeCode = gear::decode_quad( type.data() );
			
			switch ( typeCode )
			{
				case 'APPL':
					gProductType = kProductApp;
					break;
				
				case 'INIT':
				case 'DRVR':
					gProductType = kProductCodeResource;
					break;
				
				default:
					std::fprintf( stderr, "%s\n", "ld: file type in PkgInfo is not recognized" );
			}
			
			return false;  // Not a library
		}
		
		return true;
	}
	
	static void do_bare_argument( const char* arg, vxo::PtrVec_< const char >& command_args )
	{
		const bool needs_link = do_special_case_arg( arg );
		
		if ( !needs_link )
		{
			return;
		}
		
		const bool is_pathname = std::strchr( arg, '/' ) != NULL;
		
		if ( is_pathname  &&  gFirstObjectFilePath == NULL )
		{
			const size_t length = strlen( arg );
			
			const int base_length = length - STRLEN( ".o" );
			
			if ( base_length > 0  &&  memcmp( &arg[ base_length ], STR_LEN( ".o" ) ) == 0 )
			{
				gFirstObjectFilePath = arg;
				
				check_object_file( gFirstObjectFilePath );
			}
		}
		
		command_args.push_back( StoreMacPathFromPOSIXPath( is_pathname ? arg : FindSystemLibrary( arg ).c_str() ) );
	}
	
	int Main( int argc, char** argv )
	{
		vxo::PtrVec_< const char > command_args;
		
		while ( const char* arg = *++argv )
		{
			if ( arg[0] == '-' )
			{
				do_hyphen_option( argv, command_args );
			}
			else if ( arg[0] == '+' )
			{
				do_plus_option( arg, command_args );
			}
			else
			{
				do_bare_argument( arg, command_args );
			}
		}
		
		if ( output_pathname == NULL )
		{
			std::fprintf( stderr, "%s\n", "ld: -o is required" );
			
			return 1;
		}
		
		vxo::PtrVec_< const char > command;
		
		command.push_back( "tlsrvr"   );
		command.push_back( "--escape" );  // escape arguments to prevent expansion
		command.push_back( "--"       );  // stop interpreting options here
		
		switch ( arch )
		{
			default:
			case arch_none:
				std::fprintf( stderr, "%s\n", "ld: invalid architecture" );
				
				return 1;
			
			case arch_m68k:
				command.push_back( "MWLink68K" );
				command.push_back( "-model"    );
				command.push_back( gCFM68K ? "CFMflatdf" : "far" );
				break;
			
			case arch_ppc:
				command.push_back( "MWLinkPPC" );
				break;
		}
		
		switch ( gProductType )
		{
			case kProductCodeResource:
				command.push_back( "-xm"        );
				command.push_back( "c"          );
				command.push_back( "-rsrcfar"   );
				command.push_back( "-rsrcflags" );
				command.push_back( "system"     );  // FIXME: Not all code rsrc are system
				break;
			
			case kProductSharedLib:
				command.push_back( "-xm"          );
				command.push_back( "s"            );
				command.push_back( "-init"        );
				command.push_back( "__initialize" );
				command.push_back( "-term"        );
				command.push_back( "__terminate"  );
				command.push_back( "-export"      );
				command.push_back( "pragma"       );
				break;
			
			default:
			case kProductTool:
				if ( gCFM68K )
				{
					command.push_back( "-xm"      );
					command.push_back( "s"        );
				}
				else if ( arch == arch_m68k )
				{
					command.push_back( "-xm"      );
					command.push_back( "c"        );
					command.push_back( "-rsrcfar" );
					command.push_back( "-rt"      );
					command.push_back( "Tool=0"   );
				}
				else
				{
					command.push_back( "-xm"      );
					command.push_back( "s"        );
					
					// MWLinkPPC gets pissy if a shlb is larger than the default size,
					// even though the size is meaningless since this isn't an app.
					command.push_back( "-sizemin" );
					command.push_back( "4096"     );
					command.push_back( "-sizemax" );
					command.push_back( "8192"     );
					
					plus::var_string output_name;
					
					output_name  = p7::basename( output_pathname );
					output_name += ' ';
					output_name += gMacAPINames[ gMacAPI ];
					
					command.push_back( "-name" );
					command.push_back( store_string( output_name ) );
				}
				
				command.push_back( "-main"       );
				command.push_back( "_relix_main" );
				
				gFileType    = "Tool";
				gFileCreator = "Poof";
				
				break;
			
			case kProductApp:
				command.push_back( "-xm"      );
				command.push_back( "a"        );
				command.push_back( "-sizemin" );
				command.push_back( "4096"     );
				command.push_back( "-sizemax" );
				command.push_back( "8192"     );
				
				if ( arch == arch_ppc )
				{
					// CFMLateImport requires that we don't pack the data segment
					command.push_back( "-b" );
				}
				
				command.push_back( "-dead" );
				
				command.push_back( arch == arch_m68k ? "code" : "off" );
				
				break;
		}
		
		if ( gFileType )
		{
			command.push_back( "-t"      );
			command.push_back( gFileType );
		}
		
		if ( gFileCreator )
		{
			command.push_back( "-c"         );
			command.push_back( gFileCreator );
		}
		
		if ( sym )
		{
			command.push_back( "-sym" );
			command.push_back( "full" );
		}
		
		if ( debug )
		{
			if ( arch == arch_ppc )
			{
				command.push_back( "-tb" );
				command.push_back( "on"  );
			}
		}
		
		plus::string output_mac_pathname = mac_pathname_from_path( output_pathname, true );
		
		plus::string linkmap_mac_pathname = output_mac_pathname + ".map";
		
		command.push_back( "-o" );
		command.push_back( output_mac_pathname.c_str() );
		
		command.push_back( "-map" );
		command.push_back( linkmap_mac_pathname.c_str() );
		
		command.push_back( "-cmap" );
		command.push_back( "R*ch" );
		
		command.insert( command.end(), command_args.begin(), command_args.end() );
		
		int unlinked = ::unlink( output_pathname );
		
		if ( unlinked < 0  &&  errno != ENOENT )
		{
			p7::throw_errno( errno );
		}
		
		if ( verbose )
		{
			plus::var_string output = join( command.begin(), command.end(), " " );
			
			output += '\n';
			
			write( STDOUT_FILENO, output.data(), output.size() );
		}
		
		command.push_back( NULL );
		
		if ( dry_run )
		{
			return 0;
		}
		
		const bool filtering = arch == arch_ppc;
		
		int pipe_ends[2];
		
		if ( filtering )
		{
			p7::throw_posix_result( pipe( pipe_ends ) );
		}
		
		p7::pid_t tlsrvr_pid = POSEVEN_VFORK();
		
		if ( tlsrvr_pid == 0 )
		{
			if ( filtering )
			{
				close( pipe_ends[0] );  // close reader
				
				dup2( pipe_ends[1], STDERR_FILENO );
				
				close( pipe_ends[1] );  // close spare writer
			}
			
			p7::execvp( &command[0] );
		}
		
		if ( filtering )
		{
			close( pipe_ends[1] );  // close writer
			
			p7::pid_t filter_pid = POSEVEN_VFORK();
			
			if ( filter_pid == 0 )
			{
				dup2( p7::stderr_fileno, p7::stdout_fileno );  // redirect stdout to stderr
				dup2( pipe_ends[0],      p7::stdin_fileno  );  // redirect stdin to pipe reader
				
				close( pipe_ends[0] );  // close spare reader
				
				const char *const filter_argv[] = { "filter-mwlink-warnings.pl", NULL };
				
				p7::execvp( filter_argv );
			}
			
			close( pipe_ends[0] );  // close reader
			
			p7::waitpid( filter_pid );
		}
		
		p7::wait_t wait_status = p7::waitpid( tlsrvr_pid );
		
		p7::exit_t exit_status = nucleus::convert< p7::exit_t >( wait_status );
		
		if ( exit_status != 0 )
		{
			return exit_status;
		}
		
		if ( arch == arch_m68k  &&  !gCFM68K  &&  gProductType == kProductTool )
		{
			const char *const postlink_argv[] =
			{
				"postlink-68k-tool",
				sym ? output_pathname : "--data-fork",
				sym ? NULL            : output_pathname,
				NULL
			};
			
			p7::execvp( postlink_argv );
		}
		
		return exit_status;
	}
	
}
