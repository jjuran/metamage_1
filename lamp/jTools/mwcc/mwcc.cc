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


static int exit_from_wait( int stat )
{
	int result = WIFEXITED( stat )   ? WEXITSTATUS( stat )
	           : WIFSIGNALED( stat ) ? WTERMSIG( stat ) + 128
	           :                       -1;
	
	return result;
}


namespace jTools
{
	
	namespace Div = Divergence;
	
	
	static const char* kInvariantMWCOptions = " -nosyspath"
			                                  " -w all,nounusedarg,noimplicit,nonotinlined"
			                                  " -ext o"
			                                  " -maxerrors 8"
			                                  " -convertpaths -nomapcr"
			                                  " -proto strict";
	
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
		const char* dot = std::strrchr( pathname, '.' );
		
		bool header = dot[1] == 'h';
		
		return (header ? "-precompile " : "-o ") + QuotedMacPathFromPOSIXPath( pathname );
	}
	
	static int Main( int argc, const char *const argv[] )
	{
		std::string command = TARGET_CPU_68K ? "MWC68K" : "MWCPPC";
		std::string mwcArgs = kInvariantMWCOptions;
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
					
					case 'c':
						// 'gcc -c' means compile
						continue;
					
					case 'a':
						if ( std::strcmp( arg + 1, "arch" ) == 0 )
						{
							command = CommandFromArch( *++argv );
							continue;
						}
						break;
					
					case 'f':
						arg = TranslateCodeGenFlag( arg );
						break;
					
					case 'g':
						arg = "-sym full";
						break;
					
					case 'O':
						arg = arg[2] == '0' ? "-opt off"
						    : arg[2] == '4' ? "-opt full"
						    :                 arg;
						break;
					
					case 'o':
						translatedPath = OutputFile( *++argv );
						arg = translatedPath.c_str();
						break;
					
					case 'I':
						translatedPath = "-I" + QuotedMacPathFromPOSIXPath( arg + 2 );
						arg = translatedPath.c_str();
						break;
					
					case 'i':
						if ( std::strcmp( arg + 1, "include" ) == 0 )
						{
							translatedPath = "-prefix" + QuotedMacPathFromPOSIXPath( arg + 2 );
							arg = translatedPath.c_str();
						}
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
		
		if ( verbose )
		{
			write( STDOUT_FILENO, output.data(), output.size() );
		}
		
		int wait_status = system( output.c_str() );
		
		return exit_from_wait( wait_status );
	}
	
}

int O::Main( int argc, const char *const argv[] )
{
	return jTools::Main( argc, argv );
}

