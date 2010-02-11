/*	=====
 *	ld.cc
 *	=====
 */

// Standard C++
#include <list>
#include <vector>

// Standard C
#include <stdlib.h>

// iota
#include "iota/decimal.hh"
#include "iota/quad.hh"
#include "iota/strings.hh"

// text-input
#include "text_input/feed.hh"
#include "text_input/get_line_from_feed.hh"

// poseven
#include "poseven/extras/slurp.hh"
#include "poseven/functions/execvp.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/openat.hh"
#include "poseven/functions/read.hh"
#include "poseven/functions/readlinkat.hh"
#include "poseven/functions/stat.hh"
#include "poseven/functions/vfork.hh"
#include "poseven/functions/waitpid.hh"
#include "poseven/types/fd_t.hh"
#include "poseven/Pathnames.hh"

// Nitrogen
#include "Nitrogen/MacErrors.h"
#include "Nitrogen/Resources.h"

// Nitrogen Extras
#include "FSReader.hh"

// Io: MacFiles
#include "MacFiles/Classic.hh"

// GetPathname
#include "GetPathname.hh"

// Divergence
#include "Divergence/Utilities.hh"

// Orion
#include "Orion/Main.hh"

// metrowerks
#include "metrowerks/object_file.hh"


namespace tool
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace n = nucleus;
	namespace p7 = poseven;
	namespace mw = metrowerks;
	namespace Div = Divergence;
	
	
	using namespace io::path_descent_operators;
	
	
	template < class Iter >
	std::string join( Iter begin, Iter end, const std::string& glue = "" )
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
	
	
	static std::string find_ToolServer()
	{
		n::owned< p7::fd_t > vol = p7::open( "/sys/mac/vol/", p7::o_rdonly | p7::o_directory );
		
		n::owned< p7::fd_t > vol_list = p7::openat( vol, "list", p7::o_rdonly | p7::o_directory );
		
		for ( int i = 1;  ;  ++i )
		{
			const char *name = iota::inscribe_decimal( i );
			
			n::owned< p7::fd_t > list_i = p7::openat( vol_list, name, p7::o_rdonly | p7::o_directory );
			
			try
			{
				return p7::readlinkat( list_i, "dt/appls/MPSX/latest" );
			}
			catch ( const p7::errno_t& err )
			{
				if ( err != ENOENT )
				{
					throw;
				}
			}
		}
	}
	
	static std::string find_Libraries()
	{
		std::string pathname = find_ToolServer();
		
		typedef std::string::reverse_iterator Iter;
		
		Iter it = std::find( pathname.rbegin(), pathname.rend(), '/' );
		
		if ( it == pathname.rend() )
		{
			p7::throw_errno( ENOENT );
		}
		
		it = std::find( it + 1, pathname.rend(), '/' );
		
		if ( it == pathname.rend() )
		{
			p7::throw_errno( ENOENT );
		}
		
		pathname.resize( pathname.rend() - it );
		
		pathname += "Interfaces&Libraries/Libraries";
		
		return pathname;
	}
	
	static const std::string& get_Libraries_pathname()
	{
		static std::string libraries = find_Libraries();
		
		return libraries;
	}
	
	
	#define SHARED_LIB( lib )  { lib "", "SharedLibraries" }
	
	#define PPC_LIB( lib )  { lib "", "PPCLibraries" }
	
	#define MW68K_LIB( lib )  { lib "", "MW68KLibraries" }
	#define MWPPC_LIB( lib )  { lib "", "MWPPCLibraries" }
	
	typedef const char* StringPair[2];
	
	static StringPair gSystemLibraries[] =
	{
		SHARED_LIB( "AppearanceLib"      ),
		SHARED_LIB( "AppleScriptLib"     ),
		SHARED_LIB( "CarbonLib"          ),
		SHARED_LIB( "ControlsLib"        ),
		SHARED_LIB( "InterfaceLib"       ),
		SHARED_LIB( "InternetConfigLib"  ),
		SHARED_LIB( "MathLib"            ),
		SHARED_LIB( "MenusLib"           ),
		SHARED_LIB( "ObjectSupportLib"   ),
		SHARED_LIB( "OpenTptInternetLib" ),
		SHARED_LIB( "OpenTransportLib"   ),
		SHARED_LIB( "QuickTimeLib"       ),
		SHARED_LIB( "ThreadsLib"         ),
		SHARED_LIB( "WindowsLib"         ),
		
		PPC_LIB( "CarbonAccessors.o"        ),
		PPC_LIB( "CursorDevicesGlue.o"      ),
		PPC_LIB( "OpenTransportAppPPC.o"    ),
		PPC_LIB( "OpenTptInetPPC.o"         ),
		PPC_LIB( "PascalPreCarbonUPPGlue.o" ),
		
		MWPPC_LIB( "MSL C.Carbon.Lib"     ),
		MWPPC_LIB( "MSL C.PPC.Lib"        ),
		MWPPC_LIB( "MSL C++.PPC.Lib"      ),
		MWPPC_LIB( "MSL RuntimePPC.Lib"   ),
		MWPPC_LIB( "PLStringFuncsPPC.lib" ),
		
		MW68K_LIB( "MacOS.lib" ),
		
		MW68K_LIB( "MathLib68K Fa(4i_8d).Lib"    ),
		MW68K_LIB( "MathLib68K Fa(4i_8d).A4.Lib" ),
		MW68K_LIB( "MathLibCFM68K (4i_8d).Lib"   ),
		
		MW68K_LIB( "MSL C.68K Fa(4i_8d).Lib"    ),
		MW68K_LIB( "MSL C.68K Fa(4i_8d).A4.Lib" ),
		MW68K_LIB( "MSL C.CFM68K Fa(4i_8d).Lib" ),
		
		MW68K_LIB( "MSL C++.68K Fa(4i_8d).Lib"    ),
		MW68K_LIB( "MSL C++.68K Fa(4i_8d).A4.Lib" ),
		MW68K_LIB( "MSL C++.CFM68K Fa(4i_8d).Lib" ),
		
		MW68K_LIB( "MSL MWCFM68KRuntime.Lib" ),
		MW68K_LIB( "MSL Runtime68K.Lib"      ),
		MW68K_LIB( "MSL Runtime68K.A4.Lib"   ),
		
		MW68K_LIB( "PLStringFuncs.glue"      ),
		MW68K_LIB( "PLStringFuncsCFM68K.lib" ),
		
		{ NULL, NULL }
	};
	
	typedef std::map< std::string, const char* > LibraryMap;
	
	static LibraryMap MakeLibraryMap()
	{
		LibraryMap map;
		
		for ( StringPair* it = gSystemLibraries;  it[0][0] != NULL;  ++it )
		{
			map[ it[0][0] ] = it[0][1];
		}
		
		return map;
	}
	
	static LibraryMap& TheLibraryMap()
	{
		static LibraryMap gLibraryMap = MakeLibraryMap();
		
		return gLibraryMap;
	}
	
	static std::string FindSystemLibrary( const std::string& libName )
	{
		LibraryMap::const_iterator it = TheLibraryMap().find( libName );
		
		if ( it == TheLibraryMap().end() )
		{
			return libName;
		}
		
		const char* subdir = it->second;
		
		std::string pathname = get_Libraries_pathname() / subdir / libName;
		
		if ( !io::file_exists( pathname ) )
		{
			std::fprintf( stderr, "System library missing: %s\n", pathname.c_str() );
			
			throw p7::exit_failure;
		}
		
		return pathname;
	}
	
	
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
	
	static std::string MacPathFromPOSIXPath( const char* pathname )
	{
		FSSpec item = Div::ResolvePathToFSSpec( pathname );
		
		return GetMacPathname( item );
	}
	
	std::vector< const char* > gLibraryDirs;
	
	static void RememberLibraryDir( const char* pathname )
	{
		gLibraryDirs.push_back( pathname );
	}
	
	static std::string FindLibrary( const char* lib )
	{
		typedef std::vector< const char* >::const_iterator Iter;
		
		std::string filename = lib;
		
		filename += ".lib";
		
		for ( Iter it = gLibraryDirs.begin();  it != gLibraryDirs.end();  ++it )
		{
			std::string pathname = std::string( *it ) / filename;
			
			if ( io::file_exists( pathname ) )
			{
				return pathname;
			}
		}
		
		std::fprintf( stderr, "ld: can't find library: %s\n", lib );
		
		throw p7::exit_failure;
		
		// Not reached
		return filename;
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
	
	#define CODE_LABEL "Code: "
	#define CODE_FORMAT_68K  CODE_LABEL "0079b2   12345 bytes  "
	
	static unsigned long GetOffsetOfRoutine( const FSSpec& file, const char* quoted_name, std::size_t name_length )
	{
		FSSpec linkMap = file;
		
		std::memcpy( linkMap.name + 1 + linkMap.name[0], STR_LEN( ".map" ) );
		
		linkMap.name[0] += 4;
		
		const char* format = CODE_FORMAT_68K;
		
		std::size_t format_length = STRLEN( CODE_FORMAT_68K );
		
		text_input::feed feed;
		
		NN::Owned< N::FSFileRefNum > fRefNum = N::FSpOpenDF( linkMap, N::fsRdPerm );
		
		N::FSReader reader( fRefNum );
		
		const std::size_t minimum_line_length = format_length + name_length;
		
		while ( const std::string* s = get_line_from_feed( feed, reader ) )
		{
			std::string line( s->begin(), s->end() - 1 );
			
			if ( line.length() < minimum_line_length )
			{
				continue;
			}
			
			if ( std::memcmp( line.c_str(), STR_LEN( CODE_LABEL ) ) != 0 )
			{
				continue;
			}
			
			if ( std::memcmp( line.c_str() + format_length, quoted_name, name_length ) == 0 )
			{
				const char* begin = line.c_str() + STRLEN( CODE_LABEL );
				
				const unsigned long offset = std::strtoul( begin, NULL, 16 );
				
				return offset;
			}
		}
		
		return 0;
	}
	
	static void Patch68KStartupCode( ::Handle code, UInt32 initToolOffset,
	                                                UInt32 initCodeOffset,
	                                                UInt32 lampMainOffset )
	{
		const UInt32 nopnop = 0x4e714e71;
		const UInt32 jmp    = 0x4efa0000;
		const UInt32 jsr    = 0x4eba0000;
		
		UInt32* const saveRegisters = reinterpret_cast< UInt32* >( *code + 12 );
		UInt32* const setCurrentA4  = reinterpret_cast< UInt32* >( *code + 16 );
		UInt32* const loadStartToA0 = reinterpret_cast< UInt32* >( *code + 20 );
		UInt32* const moveAndStrip  = reinterpret_cast< UInt32* >( *code + 24 );
		UInt32* const setupMainRsrc = reinterpret_cast< UInt32* >( *code + 28 );
		UInt32* const restoreRegs   = reinterpret_cast< UInt32* >( *code + 32 );
		UInt32* const jmpToMain     = reinterpret_cast< UInt32* >( *code + 36 );
		
		*saveRegisters = *setCurrentA4  + 4;
		*setCurrentA4  = *loadStartToA0 + 4;
		*loadStartToA0 = *moveAndStrip;
		*moveAndStrip  = *setupMainRsrc + 4;
		*setupMainRsrc = initToolOffset ? jsr | (initToolOffset - 28 - 2) : nopnop;
		*restoreRegs   = initCodeOffset ? jsr | (initCodeOffset - 32 - 2) : nopnop;
		*jmpToMain     = lampMainOffset ? jmp | (lampMainOffset - 36 - 2) : *jmpToMain;
	}
	
	#define QUOT "\""
	
	#define QUOTED( string )  QUOT string QUOT
	
	static void Patch68KStartup( const FSSpec& file )
	{
		unsigned long inittool = GetOffsetOfRoutine( file, STR_LEN( QUOTED( "InitializeTool" ) ) );
		unsigned long initcode = GetOffsetOfRoutine( file, STR_LEN( QUOTED( "__InitCode__"   ) ) );
		unsigned long lampmain = GetOffsetOfRoutine( file, STR_LEN( QUOTED( "_lamp_main"     ) ) );
		
		if ( inittool > 0x7fff )
		{
			std::fprintf( stderr, "ld: InitTool() offset 0x%.8x is out of range for 16-bit reference\n", inittool );
			
			N::FSpDelete( file );
			
			throw p7::exit_failure;
		}
		
		N::ResType  resType = N::ResType( 'Wish' );
		N::ResID    resID   = N::ResID  ( 0      );
		
		NN::Owned< N::ResFileRefNum > resFile = N::FSpOpenResFile( file, N::fsRdWrPerm );
		
		N::Handle code = N::Get1Resource( resType, resID );
		
		Patch68KStartupCode( code.Get(), inittool, initcode, lampmain );
		
		N::ChangedResource( code );
		
		N::WriteResource( code );
	}
	
	static const char* store_string( const std::string& string )
	{
		static std::list< std::string > static_string_storage;
		
		static_string_storage.push_back( string );
		
		return static_string_storage.back().c_str();
	}
	
	static const char* StoreMacPathFromPOSIXPath( const char* pathname )
	{
		return store_string( MacPathFromPOSIXPath( pathname ) );
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
	
	int Main( int argc, iota::argv_t argv )
	{
		NN::RegisterExceptionConversion< NN::Exception, N::OSStatus >();
		
		std::vector< const char* > command_args;
		
		Architecture arch = TARGET_CPU_68K ? arch_m68k
		                  : TARGET_CPU_PPC ? arch_ppc
		                  :                  arch_none;
		
		const char* output_pathname = NULL;
		
		bool debug = true;
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
					
					case 's':
						if ( arg[2] == '\0' )
						{
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
							
							std::string library_pathname = FindLibrary( lib_name );
							
							const char* mac_pathname = StoreMacPathFromPOSIXPath( library_pathname.c_str() );
							
							// Link Orion and InitTool first, if present.
							// This hack is necessary on 68K to ensure that main()
							// and InitTool() reside within the first 32K, accessible
							// by JMP or JSR from the startup code.
							
							const bool expedited =    std::strcmp( lib_name, "Orion"    ) == 0
							                       || std::strcmp( lib_name, "InitTool" ) == 0;
							
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
			else if ( arg[0] == '+' )
			{
				if ( const char* equals = std::strchr( arg, '=' ) )
				{
					std::string option( arg, equals );
					
					option[0] = '-';
					
					command_args.push_back( store_string( option ) );
					
					command_args.push_back( equals + 1 );
				}
				else
				{
					// error
				}
			}
			else
			{
				std::string filename = io::get_filename( arg );
				
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
					std::string pkgInfo = p7::slurp( arg );
					
					if ( pkgInfo.length() < sizeof 'Type' + sizeof 'Crtr' )
					{
						std::fprintf( stderr, "%s\n", "ld: PkgInfo is shorter than 8 bytes" );
						
						throw N::EOFErr();
					}
					
					std::string type   ( pkgInfo.begin(),     pkgInfo.begin() + 4 );
					std::string creator( pkgInfo.begin() + 4, pkgInfo.begin() + 8 );
					
					gFileType    = store_string( type    );
					gFileCreator = store_string( creator );
					
					N::OSType typeCode = N::OSType( iota::decode_quad( type.data() ) );
					
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
					
					continue;
				}
				
				const bool is_pathname = std::strchr( arg, '/' ) != NULL;
				
				if ( is_pathname  &&  gFirstObjectFilePath == NULL )
				{
					const size_t length = strlen( arg );
					
					const int base_length = length - STRLEN( ".o" );
					
					if ( base_length > 0  &&  memcmp( &arg[ base_length ], STR_LEN( ".o" ) ) == 0 )
					{
						gFirstObjectFilePath = arg;
					}
				}
				
				std::string library_pathname;
				
				if ( !is_pathname )
				{
					library_pathname = FindSystemLibrary( arg );
					
					arg = library_pathname.c_str();
				}
				
				command_args.push_back( StoreMacPathFromPOSIXPath( arg ) );
			}
		}
		
		if ( output_pathname == NULL )
		{
			std::fprintf( stderr, "%s\n", "ld: -o is required" );
			
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
				std::fprintf( stderr, "%s\n", "ld: invalid architecture" );
				
				return EXIT_FAILURE;
			
			case arch_m68k:
				check_object_file( gFirstObjectFilePath );
				
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
					command.push_back( "-export"  );
					command.push_back( "sym=_lamp_main" );
				}
				else if ( arch == arch_m68k )
				{
					command.push_back( "-xm"      );
					command.push_back( "c"        );
					command.push_back( "-rsrcfar" );
					command.push_back( "-rt"      );
					command.push_back( "Wish=0"   );
				}
				else
				{
					command.push_back( "-xm"      );
					command.push_back( "s"        );
					command.push_back( "-export"  );
					command.push_back( "sym=_lamp_main" );
					
					// MWLinkPPC gets pissy if a shlb is larger than the default size,
					// even though the size is meaningless since this isn't an app.
					command.push_back( "-sizemin" );
					command.push_back( "4096"     );
					command.push_back( "-sizemax" );
					command.push_back( "8192"     );
					
					std::string output_name;
					
					output_name  = io::get_filename( output_pathname );
					output_name += ' ';
					output_name += gMacAPINames[ gMacAPI ];
					
					command.push_back( "-name" );
					command.push_back( store_string( output_name ) );
				}
				
				gFileType    = "Wish";
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
		
		if ( debug )
		{
			command.push_back( "-sym" );
			command.push_back( "full" );
			
			if ( arch == arch_ppc )
			{
				command.push_back( "-tb" );
				command.push_back( "on"  );
			}
		}
		
		FSSpec output_filespec = Div::ResolvePathToFSSpec( output_pathname );
		
		std::string output_mac_pathname = GetMacPathname( output_filespec );
		
		std::string linkmap_mac_pathname = output_mac_pathname + ".map";
		
		command.push_back( "-o" );
		command.push_back( output_mac_pathname.c_str() );
		
		command.push_back( arch == arch_ppc ? "-map" : "-mapide" );
		command.push_back( linkmap_mac_pathname.c_str() );
		
		command.push_back( "-cmap" );
		command.push_back( "R*ch" );
		
		command.insert( command.end(), command_args.begin(), command_args.end() );
		
		if ( io::file_exists( output_filespec ) )
		{
			io::delete_file( output_filespec );
		}
		
		if ( verbose )
		{
			std::string output = join( command.begin(), command.end(), " " );
			
			output += '\n';
			
			write( STDOUT_FILENO, output.data(), output.size() );
		}
		
		command.push_back( NULL );
		
		if ( dry_run )
		{
			return EXIT_SUCCESS;
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
		
		if ( arch == arch_m68k  &&  gProductType == kProductTool )
		{
			Patch68KStartup( output_filespec );
		}
		
		return exit_status;
	}
	
}

