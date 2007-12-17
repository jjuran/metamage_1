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

// POSeven
#include "POSeven/Pathnames.hh"
#include "POSeven/Stat.hh"

// Nitrogen
#include "Nitrogen/MacErrors.h"
#include "Nitrogen/Str.h"

// GetPathname
#include "GetPathname.hh"

// Divergence
#include "Divergence/Utilities.hh"

// Orion
#include "Orion/GetOptions.hh"
#include "Orion/Main.hh"


namespace N = Nitrogen;
namespace NN = Nucleus;
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
		return "-o " + QuotedMacPathFromPOSIXPath( pathname );
	}
	
	std::vector< const char* > gLibraryDirs;
	
	static void RememberLibraryDir( const char* pathname )
	{
		gLibraryDirs.push_back( pathname );
	}
	
	static std::string FindLibrary( const char* lib )
	{
		typedef std::vector< const char* >::const_iterator Iter;
		
		for ( Iter it = gLibraryDirs.begin();  it != gLibraryDirs.end();  ++it )
		{
			std::string pathname = std::string( *it ) / lib;
			
			if ( io::file_exists( pathname ) )
			{
				return pathname;
			}
		}
		
		return FindSystemLibrary( lib );
	}
	
	enum ProductType
	{
		kProductUnknown,
		kProductStaticLib,
		kProductSharedLib,
		kProductTool,
		kProductApp
	};
	
	static ProductType gProductType = kProductUnknown;
	
	static std::string ProductOptionsForTool( bool cfm )
	{
		std::string build = cfm ? "-xm s -init InitializeFragment -term TerminateFragment -export pragma -sizemin 4096 -sizemax 8192"
		                        : "-xm c -rsrcfar -rsrcflags system -rt Wish=0";
		
		return build + " -t Wish -c Poof";
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
						
						if ( std::strcmp( arg + 1, "static" ) == 0 )
						{
							gProductType = kProductStaticLib;
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
						continue;
					
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
					foundLib = FindLibrary( arg );
					
					arg = foundLib.c_str();
				}
				
				//if ( pathname )
				{
					// translate path
					translatedPath = QuotedMacPathFromPOSIXPath( arg );
					
					arg = translatedPath.c_str();
				}
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
		
		if ( verbose )
		{
			write( STDOUT_FILENO, output.data(), output.size() );
		}
		
		int wait_status = system( output.c_str() );
		
		return exit_from_wait( wait_status );
	}
	
}

int O::Main( int argc, argv_t argv )
{
	return jTools::Main( argc, argv );
}

