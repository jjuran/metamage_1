/*
	mwcc.cc
	-------
*/

// Standard C
#include <stdio.h>
#include <string.h>

// plus
#include "plus/var_string.hh"
#include "plus/string/concat.hh"

// vxo
#include "vxo/ptrvec.hh"

// poseven
#include "poseven/functions/execvp.hh"
#include "poseven/functions/stat.hh"
#include "poseven/functions/vfork.hh"
#include "poseven/functions/wait.hh"
#include "poseven/functions/write.hh"

// pfiles
#include "pfiles/common.hh"

// Orion
#include "Orion/Main.hh"


#define UTF8_ZERO_WIDTH_SPACE  "\xe2\x80\x8b"
#define UTF8_PILCROW           "\xc2\xb6"

#define PATH_MARKER  UTF8_ZERO_WIDTH_SPACE UTF8_PILCROW

#define COMMON_WARNINGS  "all,nounusedarg,nonotinlined,noextracomma,"

/*
	CodeWarrior Pro 6 takes `implicit` as an abbreviation of `implicitconv`,
	while Pro 4 only accepts the latter.  Whereas it's common to warn about
	"possible unwanted assignment" for `if (dst = src)`, modern compilers
	(and even CW Pro 6) suppress that warning for `if ((dst = src))`, where
	the extra parentheses are construed to indicate that the assignment is
	intended.  Pro 4 doesn't recognize that idiom, so disable such warnings.
*/

#ifdef __MC68K__
#define WARNINGS COMMON_WARNINGS "noimplicitconv,nopossible"
#else
#define WARNINGS COMMON_WARNINGS "noimplicit"
#endif


namespace tool
{
	
	namespace n = nucleus;
	namespace p7 = poseven;
	
	
	using namespace io::path_descent_operators;
	
	
	typedef vxo::UPtrVec< const char > CStrVec;
	
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
	
	
	enum Architecture
	{
		arch_none,
		arch_m68k,
		arch_ppc
	};
	
	static Architecture read_arch( const char* arch )
	{
		if ( strcmp( arch, "m68k" ) == 0 )
		{
			return arch_m68k;
		}
		
		if ( strcmp( arch, "ppc" ) == 0 )
		{
			return arch_ppc;
		}
		
		return arch_none;
	}
	
	static bool rtti = true;
	static bool cfm = false;
	static bool mc68020 = false;
	static bool a4 = false;
	static bool pc_rel = false;
	
	static void set_codegen_flag( const plus::string& flag )
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
		else if ( flag == "-m68020" )
		{
			mc68020 = true;
		}
		else if ( flag == "-mA4-globals" )
		{
			a4 = true;
		}
		else if ( flag == "-mpcrel" )
		{
			pc_rel = true;
		}
	}
	
	static bool extension_begins_with_char( const char* path, char c )
	{
		const char* dot   = strrchr( path, '.' );
		const char* slash = strrchr( path, '/' );
		
		const bool has_dot = dot > slash;  // Works even if slash or dot is NULL
		
		return has_dot  &&  dot[ 1 ] == c;
	}
	
	static inline bool is_object_filename( const char* path )
	{
		return extension_begins_with_char( path, 'o' );
	}
	
	CStrVec gIncludeDirs;
	
	static void RememberIncludeDir( const char* pathname )
	{
		gIncludeDirs.push_back( pathname );
	}
	
	static plus::string get_prefix_image_path( const char* prefix_path )
	{
		const bool header_is_source = extension_begins_with_char( prefix_path, 'h' );
		
		if ( !header_is_source )
		{
			return prefix_path;
		}
		
		plus::var_string prefix_image_path = prefix_path;
		
		prefix_image_path += ".mwch";
		
		const bool filename_only = strchr( prefix_path, '/' ) == NULL;
		
		if ( !filename_only )
		{
			return prefix_image_path;
		}
		
		typedef CStrVec::const_iterator Iter;
		
		for ( Iter it = gIncludeDirs.begin();  it != gIncludeDirs.end();  ++it )
		{
			plus::string location = *it / prefix_image_path;
			
			if ( io::file_exists( location ) )
			{
				return location;
			}
		}
		
		// FIXME:  Should probably throw ENOENT
		return prefix_image_path;
	}
	
	
	int Main( int argc, char** argv )
	{
		CStrVec command_args;
		
		Architecture arch = arch_none;
		
	#ifdef __MC68K__
		
		arch = arch_m68k;
		
	#endif
		
	#ifdef __POWERPC__
		
		arch = arch_ppc;
		
	#endif
		
		const char* output_pathname = NULL;
		
		const char* opt = NULL;
		
		bool sym     = false;
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
						if ( strcmp( arg + 1, "arch" ) == 0 )
						{
							arch = read_arch( *++argv );
						}
						break;
					
					case 'm':
					case 'f':
						set_codegen_flag( arg );
						break;
					
					case 'g':
						sym = true;
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
						
						command_args.push_back( PATH_MARKER "-I" );
						command_args.push_back( arg + 2 );
						break;
					
					case 'i':
						if ( strcmp( arg + 1, "include" ) == 0 )
						{
							plus::string prefix_path = get_prefix_image_path( *++argv );
							
							command_args.push_back( "-prefix" );
							
							command_args.push_back( PATH_MARKER );
							command_args.push_back( prefix_path.c_str() );
						}
						break;
					
					default:
						break;
				}
			}
			else
			{
				command_args.push_back( PATH_MARKER );
				command_args.push_back( arg );
			}
		}
		
		if ( output_pathname == NULL )
		{
			fprintf( stderr, "%s\n", "mwcc: -o is required" );
			
			return 1;
		}
		
		CStrVec command;
		
		command.push_back( "tlsrvr"   );
		command.push_back( "--escape" );  // escape arguments to prevent expansion
		command.push_back( "--"       );  // stop interpreting options here
		
		switch ( arch )
		{
			default:
			case arch_none:
				fprintf( stderr, "%s\n", "mwcc: invalid architecture" );
				
				return 1;
			
			case arch_m68k:
				command.push_back( "MWC68K"   );
				
				if ( mc68020 )
				{
					command.push_back( "-mc68020" );
				}
				
				if ( pc_rel )
				{
					command.push_back( "-b" );
				}
				else
				{
					command.push_back( "-model" );
					command.push_back( cfm ? "CFMflatdf" : "far" );
				}
				
				if ( a4 )
				{
					command.push_back( "-a4" );
				}
				
				break;
			
			case arch_ppc:
				command.push_back( "MWCPPC" );
				break;
		}
		
		command.push_back( is_object_filename( output_pathname ) ? "-o" : "-precompile" );
		
		command.push_back( PATH_MARKER );
		command.push_back( output_pathname );
		
		if ( !rtti )
		{
			command.push_back( "-RTTI" );
			command.push_back( "off"   );
		}
		
		if ( sym )
		{
			command.push_back( "-sym" );
			command.push_back( "full" );
		}
		
		const bool debug = opt == NULL  ||  opt[2] <= '2';
		
		if ( arch == arch_m68k  &&  !debug )
		{
			command.push_back( "-mbg" );
			command.push_back( "off"  );
		}
		
		if ( arch == arch_ppc )
		{
			command.push_back( "-tb" );
			command.push_back( "on"  );
		}
		
		const char* opt_level = sym   ? "off"
		                      : debug ? "on"
		                      :         "full";
		
		command.push_back( "-opt" );
		command.push_back( opt_level );
		
		if ( !gIncludeDirs.empty() )
		{
			command.push_back( "-nosyspath"      );
			command.push_back( "-convertpaths"   );
			command.push_back( "-once"           );
		}
		
		command.push_back( "-nomapcr"        );
		
		command.push_back( "-w" );
		command.push_back( WARNINGS );
		
		command.push_back( "-ext" );
		command.push_back( "o"    );
		
		command.push_back( "-maxerrors" );
		command.push_back( "8"          );
		
		command.push_back( "-proto" );
		command.push_back( "strict" );
		
		command.insert( command.end(), command_args.begin(), command_args.end() );
		
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
