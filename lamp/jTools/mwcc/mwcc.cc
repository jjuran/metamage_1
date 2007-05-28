/*	=======
 *	mwcc.cc
 *	=======
 */

// Standard C++
#include <vector>

// Standard C
#include <stdlib.h>

// POSIX
#include <sys/wait.h>

// POSeven
#include "POSeven/Pathnames.hh"

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
	
	using namespace io::path_descent_operators;
	
	
	static const char* kInvariantMWCOptions = " -nosyspath"
			                                  " -w all,nounusedarg,noimplicit,nonotinlined"
			                                  " -ext o"
			                                  " -maxerrors 8"
			                                  " -convertpaths -nomapcr"
			                                  " -proto strict";
	
	static bool m68k = false;
	static bool ppc  = false;
	
	static std::string CommandFromArch( const std::string& arch )
	{
		if ( arch == "m68k" )
		{
			m68k = true;
			
			return "MWC68K -mbg off -model far";
		}
		else if ( arch == "ppc" )
		{
			ppc = true;
			
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
		else if ( flag == "-mCFM"  &&  m68k )
		{
			return "-model CFMflatdf";
		}
		else if ( flag == "-mA4-globals" )
		{
			return "-a4";
		}
		else if ( flag == "-ftraceback" )
		{
			return "-tb on";
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
		const char* dot   = std::strrchr( pathname, '.' );
		const char* slash = std::strrchr( pathname, '/' );
		
		bool hasDot = dot > slash;  // Works even if slash or dot is NULL
		
		bool objectCode = !hasDot || dot[1] == 'o';
		
		return (objectCode ? "-o " : "-precompile ") + QuotedMacPathFromPOSIXPath( pathname );
	}
	
	std::vector< const char* > gIncludeDirs;
	
	static void RememberIncludeDir( const char* pathname )
	{
		gIncludeDirs.push_back( pathname );
	}
	
	static std::string PrecompiledHeader( const char* pathname )
	{
		const char* dot   = std::strrchr( pathname, '.' );
		const char* slash = std::strrchr( pathname, '/' );
		
		bool hasDot = dot > slash;  // Works even if slash or dot is NULL
		
		bool headerIsSource = hasDot  &&  dot[1] == 'h';
		
		std::string precompiledHeaderImage;
		
		if ( headerIsSource )
		{
			bool pathSpecified = slash != NULL;
			
			if ( pathSpecified )
			{
				precompiledHeaderImage = pathname;
				precompiledHeaderImage += ".mwch";
			}
			else
			{
				std::string filename = io::get_filename( pathname );
				
				filename += ".mwch";
				
				typedef std::vector< const char* >::const_iterator Iter;
				
				for ( Iter it = gIncludeDirs.begin();  it != gIncludeDirs.end();  ++it )
				{
					std::string location = *it / filename;
					
					if ( io::file_exists( location ) )
					{
						precompiledHeaderImage = location;
						break;
					}
				}
			}
			
			pathname = precompiledHeaderImage.c_str();
		}
		else
		{
			//precompiledHeaderImage = pathname;
		}
		
		return "-prefix " + QuotedMacPathFromPOSIXPath( pathname );
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
					
					case 'm':
						if ( std::strcmp( arg + 1, "model" ) == 0 )
						{
							break;
						}
						// fall through
					case 'f':
						arg = TranslateCodeGenFlag( arg );
						break;
					
					case 'g':
						arg = ppc ? "-sym full" : "-sym full -mbg full";
						break;
					
					case 'O':
						arg = arg[2] == '0' ? "-opt off"
						    : arg[2] == '4' ? "-opt full"
						    :                 arg;
						break;
					
					case 'o':
						if ( arg[2] == '\0' )
						{
							translatedPath = OutputFile( *++argv );
							arg = translatedPath.c_str();
						}
						break;
					
					case 'I':
						RememberIncludeDir( arg + 2 );
						translatedPath = "-I" + QuotedMacPathFromPOSIXPath( arg + 2 );
						arg = translatedPath.c_str();
						break;
					
					case 'i':
						if ( std::strcmp( arg + 1, "include" ) == 0 )
						{
							translatedPath = PrecompiledHeader( *++argv );
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
		
		std::string output = "tlsrvr --escape -- " + command + mwcArgs + '\n';
		
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

