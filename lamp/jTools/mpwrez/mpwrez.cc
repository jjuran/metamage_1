/*	=========
 *	mpwrez.cc
 *	=========
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
#include "Orion/Main.hh"


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
	
	
	static std::string MacPathFromPOSIXPath( const char* pathname )
	{
		FSSpec item = Div::ResolvePathToFSSpec( pathname );
		
		return GetMacPathname( item );
	}
	
	static std::string QuotedMacPathFromPOSIXPath( const char* pathname )
	{
		return "'" + MacPathFromPOSIXPath( pathname ) + "'";
	}
	
	static std::string PathnameOption( const char* option, const char* pathname )
	{
		return std::string( option ) + " " + QuotedMacPathFromPOSIXPath( pathname );
	}
	
	std::vector< const char* > gIncludeDirs;
	
	static int Main( int argc, const char *const argv[] )
	{
		std::string command = "Rez";
		std::string rezArgs;
		std::string translatedPath;
		
		bool verbose = false;
		
		while ( const char* arg = *++argv )
		{
			if ( arg[0] == '-' )
			{
				switch ( arg[1] )
				{
					case 'i':
					case 'o':
						if ( arg[2] == '\0' )
						{
							translatedPath = PathnameOption( arg, *++argv );
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
				rezArgs += ' ';
				rezArgs += arg;
			}
		}
		
		std::string output = "tlsrvr --switch --escape -- " + command + rezArgs + '\n';
		
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

