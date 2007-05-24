/*	=======
 *	mwcc.cc
 *	=======
 */

// Standard C++
#include <vector>

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
#include "Orion/StandardIO.hh"


namespace O = Orion;

namespace jTools
{
	
	namespace Div = Divergence;
	
	
	static std::string CommandFromArch( const std::string& arch )
	{
		if ( arch == "m68k" )
		{
			return "MWC68K";
		}
		else if ( arch == "ppc" )
		{
			return "MWCPPC";
		}
		
		return "MWCUnsupportedArchitecture";
	}
	
	static const char* TranslateCodeGenFlag( const std::string& flag )
	{
		if ( flag == "-fpascal-strings" )
		{
			return "";
		}
		else if ( flag == "-fno-rtti" )
		{
			return "-RTTI off";
		}
		
		return "";
	}
	
	/*
	enum
	{
		optCompile, 
		optDebug, 
		optOutput,
		optIncludePrefix
	};
	
	static O::Options DefineOptions()
	{
		O::Options options;
		
		options.DefineSetFlag( "-c", optCompile );
		options.DefineSetFlag( "-g", optDebug );
		options.DefineSetString( "-o", optOutput );
		
		options.DefineSetString( "-include", optIncludePrefix );
		
		return options;
	}
	*/
	
	static std::string MacPathFromPOSIXPath( const char* pathname )
	{
		FSSpec item = Div::ResolvePathToFSSpec( pathname );
		
		return GetMacPathname( item );
	}
	
	static std::string QuotedMacPathFromPOSIXPath( const char* pathname )
	{
		return "'" + MacPathFromPOSIXPath( pathname ) + "'";
	}
	
	static int Main( int argc, const char *const argv[] )
	{
		/*
		O::Options options = DefineOptions();
		options.GetOptions( argc, argv );
		
		const std::vector< const char* >& params = options.GetFreeParams();
		
		if ( params.size() == 0 )
		{
			Io::Err <<  "Usage:  ??\n";
			return 0;
		}
		*/
		
		std::string command = TARGET_CPU_68K ? "MWC68K" : "MWCPPC";
		std::string mwcArgs;
		std::string translatedPath;
		
		while ( const char* arg = *++argv )
		{
			if ( arg[0] == '-' )
			{
				switch ( arg[1] )
				{
					case 'c':
						continue;
					
					case 'a':
						command = CommandFromArch( *++argv );
						continue;
					
					case 'f':
						arg = TranslateCodeGenFlag( arg );
						break;
					
					case 'o':
						translatedPath = "-o " + QuotedMacPathFromPOSIXPath( *++argv );
						arg = translatedPath.c_str();
						break;
					
					case 'I':
						translatedPath = "-I" + QuotedMacPathFromPOSIXPath( arg + 2 );
						arg = translatedPath.c_str();
						break;
					
					default:
						break;
				}
			}
			else if ( std::strchr( arg, '/' ) )
			{
				// translate path
				translatedPath = QuotedMacPathFromPOSIXPath( arg );
				
				arg = translatedPath.c_str();
			}
			
			if ( arg[0] != '\0' )
			{
				mwcArgs += ' ';
				mwcArgs += arg;
			}
		}
		
		std::string output = "tlsrvr -- " + command + mwcArgs + '\n';
		
		write( STDOUT_FILENO, output.data(), output.size() );
		
		return 0;
	}
	
}

int O::Main( int argc, const char *const argv[] )
{
	return jTools::Main( argc, argv );
}

