/*	=====
 *	ld.cc
 *	=====
 */

// Standard C++
#include <vector>

// Standard C
#include <stdlib.h>

// POSIX
#include <sys/wait.h>

// Iota
#include "iota/strings.hh"

// POSeven
#include "POSeven/Pathnames.hh"
#include "POSeven/Stat.hh"

// Nitrogen
#include "Nitrogen/MacErrors.h"
#include "Nitrogen/Str.h"

// Io
#include "Io/TextInput.hh"

// GetPathname
#include "GetPathname.hh"

// Divergence
#include "Divergence/Utilities.hh"

// Orion
#include "Orion/GetOptions.hh"
#include "Orion/Main.hh"


namespace N = Nitrogen;
namespace NN = Nucleus;
namespace p7 = poseven;
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
	namespace Div = Divergence;
	
	using namespace io::path_descent_operators;
	
	
	static std::string InterfacesAndLibraries()
	{
		const N::OSType sigMPWShell = N::OSType( 'MPS ' );
		
		static std::string intfsAndLibs( GetPOSIXPathname( io::get_preceding_directory( io::get_preceding_directory( N::DTGetAPPL( sigMPWShell ) ) ) / "Interfaces&Libraries" / "Libraries" ) );
		
		return intfsAndLibs;
	}
	
	
	#define SHARED_LIB( lib )  { lib "", "SharedLibraries" }
	
	#define PPC_LIB( lib )  { lib "", "PPCLibraries" }
	
	#define MW68K_LIB( lib )  { lib "", "MW68KLibraries" }
	#define MWPPC_LIB( lib )  { lib "", "MWPPCLibraries" }
	
	typedef const char* StringPair[2];
	
	static StringPair gSystemLibraries[] =
	{
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
		
		std::string pathname = InterfacesAndLibraries() / subdir / libName;
		
		if ( !io::file_exists( pathname ) )
		{
			std::fprintf( stderr, "System library missing: %s\n", pathname.c_str() );
			
			O::ThrowExitStatus( 1 );
		}
		
		return pathname;
	}
	
	
	static bool m68k = TARGET_CPU_68K;
	static bool ppc  = TARGET_CPU_PPC;
	
	static bool debug = true;
	
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
	
	static std::string outputFilename;
	
	static FSSpec gOutputFile;
	
	
	static std::string CommandFromArch( const std::string& arch )
	{
		if ( arch == "m68k" )
		{
			m68k = true;
			ppc = false;
			
			return "MWLink68K -model far";
		}
		else if ( arch == "ppc" )
		{
			ppc = true;
			m68k = false;
			
			return "MWLinkPPC";
		}
		
		return "MWLinkUnsupportedArchitecture";
	}
	
	static std::string MacPathFromPOSIXPath( const char* pathname )
	{
		FSSpec item = Div::ResolvePathToFSSpec( pathname );
		
		return GetMacPathname( item );
	}
	
	static std::string QuotedMacPathFromPOSIXPath( const char* pathname )
	{
		return "'" + MacPathFromPOSIXPath( pathname ) + "'";
	}
	
	static std::string OutputFile( const char* pathname )
	{
		gOutputFile = Div::ResolvePathToFSSpec( pathname );
		
		std::string macPathname = GetMacPathname( gOutputFile );
		
		std::string map = ppc ? "-map" : "-mapide";
		
		return "-o '" + macPathname + "' " + map + " '" + macPathname + ".map' -cmap 'R*ch'";
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
		
		O::ThrowExitStatus( 1 );
		
		// Not reached
		return filename;
	}
	
	enum ProductType
	{
		kProductUnknown,
		kProductSharedLib,
		kProductTool,
		kProductApp
	};
	
	static ProductType gProductType = kProductUnknown;
	
	static std::string ProductOptionsForTool( bool cfm )
	{
		std::string build = cfm ? "-xm s -init InitializeFragment -term TerminateFragment -export sym=main -sizemin 4096 -sizemax 8192"
		                        : "-xm c -rsrcfar -rsrcflags system -rt Wish=0";
		
		return build + " -t Wish -c Poof";
	}
	
	static unsigned long GetOffsetOfRoutine( const FSSpec& file, const char* quoted_name )
	{
		FSSpec linkMap = file;
		
		const char* dotMap = ".map";
		
		std::copy( dotMap, dotMap + 4, linkMap.name + 1 + linkMap.name[0] );
		
		linkMap.name[0] += 4;
		
		Io::TextInputAdapter< NN::Owned< N::FSFileRefNum > > input( io::open_for_reading( linkMap ) );
		
		std::string code = "Code: ";
		
		while ( input.Ready() )
		{
			std::string line = input.Read();
			
			if ( line.find( quoted_name ) == line.npos )  continue;
			
			if ( std::equal( code.begin(), code.end(), line.begin() ) )
			{
				unsigned long offset = std::strtoul( line.c_str() + code.size(), NULL, 16 );
				
				return offset;
			}
		}
		
		return 0;
	}
	
	static unsigned long GetOffsetOfInitTool( const FSSpec& file )
	{
		return GetOffsetOfRoutine( file, "\"InitializeTool\"" );
	}
	
	static unsigned long GetOffsetOfInitCode( const FSSpec& file )
	{
		return GetOffsetOfRoutine( file, "\"__InitCode__\"" );
	}
	
	static void Patch68KStartupCode( ::Handle code, UInt32 initToolOffset, UInt32 initCodeOffset )
	{
		const UInt32 nopnop = 0x4e714e71;
		const UInt32 jsr    = 0x4eba0000;
		
		UInt32* const saveRegisters = reinterpret_cast< UInt32* >( *code + 12 );
		UInt32* const setCurrentA4  = reinterpret_cast< UInt32* >( *code + 16 );
		UInt32* const loadStartToA0 = reinterpret_cast< UInt32* >( *code + 20 );
		UInt32* const moveAndStrip  = reinterpret_cast< UInt32* >( *code + 24 );
		UInt32* const setupMainRsrc = reinterpret_cast< UInt32* >( *code + 28 );
		UInt32* const restoreRegs   = reinterpret_cast< UInt32* >( *code + 32 );
		
		*saveRegisters = *setCurrentA4  + 4;
		*setCurrentA4  = *loadStartToA0 + 4;
		*loadStartToA0 = *moveAndStrip;
		*moveAndStrip  = *setupMainRsrc + 4;
		*setupMainRsrc = initToolOffset ? jsr | (initToolOffset - 28 - 2) : nopnop;
		*restoreRegs   = initCodeOffset ? jsr | (initCodeOffset - 32 - 2) : nopnop;
	}
	
	static void Patch68KStartup( const FSSpec& file )
	{
		unsigned long inittool = GetOffsetOfInitTool( file );
		unsigned long initcode = GetOffsetOfInitCode( file );
		
		if ( inittool > 0x7fff )
		{
			std::fprintf( stderr, "ld: InitTool() offset 0x%.8x is out of range for 16-bit reference\n", inittool );
			
			N::FSpDelete( file );
			
			O::ThrowExitStatus( 1 );
		}
		
		N::ResType  resType = N::ResType( 'Wish' );
		N::ResID    resID   = N::ResID  ( 0      );
		
		NN::Owned< N::ResFileRefNum > resFile = N::FSpOpenResFile( file, N::fsRdWrPerm );
		
		N::Handle code = N::Get1Resource( resType, resID );
		
		Patch68KStartupCode( code.Get(), inittool, initcode );
		
		N::ChangedResource( code );
		
		N::WriteResource( code );
	}
	
	static int Main( int argc, iota::argv_t argv )
	{
		NN::RegisterExceptionConversion< NN::Exception, N::OSStatus >();
		
		std::string command = TARGET_CPU_68K ? "MWLink68K -model far" : "MWLinkPPC";
		std::string product = "-xm l";
		std::string ldArgs;
		std::string translatedPath;
		
		bool verbose = false;
		
		while ( const char* arg = *++argv )
		{
			if ( arg[0] == '-' )
			{
				switch ( arg[1] )
				{
					case 'v':
						verbose = true;
						continue;
					
					case 'a':
						if ( std::strcmp( arg + 1, "arch" ) == 0 )
						{
							command = CommandFromArch( *++argv );
							continue;
						}
						break;
					
					case 'd':
						if ( std::strcmp( arg + 1, "dynamic" ) == 0 )
						{
							gProductType = kProductSharedLib;
							product = "-xm s -init __initialize -term __terminate -export pragma";
							continue;
						}
						break;
					
					case 'e':
						if ( std::strcmp( arg + 1, "execute" ) == 0 )
						{
							gProductType = kProductTool;
							product = ProductOptionsForTool( ppc );
							continue;
						}
						break;
					
					case 'b':
						if ( std::strcmp( arg + 1, "bundle" ) == 0 )
						{
							gProductType = kProductApp;
							
							product = "-xm a -sizemin 4096 -sizemax 8192";
							
							if ( ppc )
							{
								// 	For CFMLateImport support
								product += " -b"                                     // don't pack the data segment
								           " -init Initialize_SavedCFragInitBlock";  // save fragment data for later
							}
							
							continue;
						}
						break;
					
					case 's':
						if ( arg[2] == '\0' )
						{
							debug = false;
							continue;
						}
						
						break;
					
					case 'c':
					case 't':
						if ( arg[2] == '\0' )
						{
							translatedPath = std::string( arg ) + " '" + *++argv + "'";
							arg = translatedPath.c_str();
						}
						break;
					
					case 'r':
						if ( arg[2] == 't'  &&  arg[3] == '\0' )
						{
							translatedPath = std::string( arg ) + " '" + *++argv + "'";
							arg = translatedPath.c_str();
						}
						break;
					
					case 'o':
						if ( arg[2] == '\0' )
						{
							translatedPath = OutputFile( *++argv );
							
							outputFilename = io::get_filename( *argv );
							
							arg = translatedPath.c_str();
							break;
						}
						
						if ( std::strcmp( arg + 1, "object" ) == 0 )
						{
							product = "-xm c -rsrcfar -rsrcflags system";
							continue;
						}
						
						break;
					
					case 'l':
						translatedPath = FindLibrary( arg + 2 );
						
						translatedPath = QuotedMacPathFromPOSIXPath( translatedPath.c_str() );
						
						// Link Orion and InitTool first, if present.
						// This hack is necessary on 68K to ensure that main()
						// and InitTool() reside within the first 32K, accessible
						// by JMP or JSR from the startup code.
						
						if (    std::strcmp( arg + 2, "Orion"    ) == 0
						     || std::strcmp( arg + 2, "InitTool" ) == 0 )
						{
							ldArgs = " " + translatedPath + ldArgs;
							
							continue;
						}
						
						arg = translatedPath.c_str();
						
						break;
					
					case 'L':
						RememberLibraryDir( arg + 2 );
						continue;
					
					default:
						break;
				}
			}
			else
			{
				if ( io::get_filename( arg ) == "CarbonLib" )
				{
					gMacAPI = kMacAPICarbon;
				}
				else if ( io::get_filename( arg ) == "InterfaceLib" )
				{
					gMacAPI = kMacAPIBlue;
				}
				
				bool pathname = std::strchr( arg, '/' ) != NULL;
				
				std::string foundLib;
				
				if ( !pathname )
				{
					foundLib = FindSystemLibrary( arg );
					
					arg = foundLib.c_str();
				}
				
				// translate path
				translatedPath = QuotedMacPathFromPOSIXPath( arg );
				
				arg = translatedPath.c_str();
			}
			
			if ( arg[0] != '\0' )
			{
				ldArgs += ' ';
				ldArgs += arg;
			}
		}
		
		std::string debugging;
		
		if ( debug )
		{
			debugging = " -sym full";
			
			if ( ppc )
			{
				debugging += " -tb on";
			}
		}
		
		std::string deadstripping;
		
		if ( gProductType == kProductApp )
		{
			deadstripping = m68k ? " -dead code" : " -dead off";
		}
		
		std::string fragmentName;
		
		if ( !m68k  &&  gProductType == kProductTool )
		{
			fragmentName = " -name '" + outputFilename + " " + gMacAPINames[ gMacAPI ] + "'";
		}
		
		ldArgs = " " + product + debugging + deadstripping + fragmentName + " " + ldArgs;
		
		std::string output = "tlsrvr --switch --escape -- " + command + ldArgs + '\n';
		
		if ( io::file_exists( gOutputFile ) )
		{
			io::delete_file( gOutputFile );
		}
		
		if ( verbose )
		{
			write( STDOUT_FILENO, output.data(), output.size() );
		}
		
		int wait_status = system( output.c_str() );
		
		int exit_status = exit_from_wait( wait_status );
		
		if ( exit_status != 0 )
		{
			return exit_status;
		}
		
		if ( m68k  &&  gProductType == kProductTool )
		{
			Patch68KStartup( gOutputFile );
		}
		
		return exit_status;
	}
	
}

int O::Main( int argc, argv_t argv )
{
	return jTools::Main( argc, argv );
}

