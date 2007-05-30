// ===========
// Builtins.cc
// ===========

#include "Builtins.hh"

// Standard C++
#include <set>

// Standard C/C++
#include <cstring>

// Standard C
#include "stdlib.h"

// POSIX
#include "fcntl.h"
#include "unistd.h"

// POSeven
#include "POSeven/Open.hh"

// Orion
#include "Orion/Main.hh"
#include "SystemCalls.hh"

// sh
#include "Options.hh"
#include "PositionalParameters.hh"
#include "Execution.hh"
#include "ReadExecuteLoop.hh"


namespace NN = Nucleus;
namespace P7 = POSeven;
namespace O = Orion;


static StringMap gLocalVariables;
static StringMap gAliases;

static std::set< std::string > gVariablesToExport;

static void PrintVariable( const StringMap::value_type& var )
{
	const std::string& name  = var.first;
	const std::string& value = var.second;
	
	std::printf( "%s='%s'\n", name.c_str(), value.c_str() );
}

static void PrintAlias( const StringMap::value_type& var )
{
	const std::string& name  = var.first;
	const std::string& value = var.second;
	
	std::printf( "alias %s='%s'\n", name.c_str(), value.c_str() );
}


static bool UnmarkVariableForExport( const std::string& name )
{
	std::set< std::string >::iterator found = gVariablesToExport.find( name );
	bool wasMarked = found != gVariablesToExport.end();
	
	if ( wasMarked )
	{
		gVariablesToExport.erase( found );
	}
	
	return wasMarked;
}

int AssignShellVariable( const char* name, const char* value )
{
	if ( getenv( name ) || UnmarkVariableForExport( name ) )
	{
		// Variable already exists in environment, or was marked for export
		setenv( name, value, 1 );
	}
	else
	{
		// Not set, or set locally
		gLocalVariables[ name ] = value;
	}
	
	return 0;
}

const char* QueryShellVariable( const std::string& name )
{
	StringMap::const_iterator found = gLocalVariables.find( name );
	
	if ( found != gLocalVariables.end() )
	{
		return found->second.c_str();
	}
	
	return NULL;
}

// Builtins.  argc is guaranteed to be positive.

static int Builtin_CD( int argc, char const* const argv[] )
{
	const char* dir = argv[1];
	
	if ( argc == 1 )
	{
		if ( const char* home = getenv( "HOME" ) )
		{
			dir = home;
		}
		else
		{
			dir = "/";
		}
	}
	
	chdir( dir );
	
	// Apparently setenv() breaks something.
	//setenv( "OLDPWD", getenv( "PWD" ), 1 );
	
	// FIXME:  This should be full pathname.
	//setenv( "PWD", argv[ 1 ], 1 );
	
	return 0;
}

static int Builtin_Alias( int argc, char const* const argv[] )
{
	if ( argc == 1 )
	{
		// $ alias
		std::for_each( gAliases.begin(),
		               gAliases.end(),
		               std::ptr_fun( PrintAlias ) );
	}
	else if ( argc == 2 )
	{
		const char* name = argv[ 1 ];
		
		if ( char* eq = std::strchr( argv[ 1 ], '=' ) )
		{
			// $ alias foo=bar
			
			// FIXME:  This is const.
			*eq = '\0';
			
			gAliases[ name ] = eq + 1;
		}
		else
		{
			// $ alias foo
			StringMap::const_iterator found = gAliases.find( name );
			
			if ( found != gAliases.end() )
			{
				PrintAlias( *found );
			}
		}
	}
	
	return 0;
}

static int Builtin_Echo( int argc, char const* const argv[] )
{
	if ( argc > 1 )
	{
		std::printf( "%s", argv[1] );
		
		for ( short i = 2; i < argc; i++ )
		{
			std::printf( " %s", argv[i] );
		}
	}
	
	std::printf( "\n" );
	
	return 0;
}

static int Builtin_Exit( int argc, char const* const argv[] )
{
	int exitStatus = 0;
	
	if ( argc > 1 )
	{
		exitStatus = std::atoi( argv[ 1 ] );
	}
	
	O::ThrowExitStatus( exitStatus );
	
	// Not reached
	return -1;
}

static int Builtin_Export( int argc, char const* const argv[] )
{
	if ( argc == 1 )
	{
		// $ export
		char** envp = environ;
		
		while ( *envp != NULL )
		{
			std::printf( "export %s\n", *envp );
			++envp;
		}
	}
	else if ( argc == 2 )
	{
		const char* const arg1 = argv[ 1 ];
		
		if ( char* eq = std::strchr( arg1, '=' ) )
		{
			// $ export foo=bar
			putenv( arg1 );
			
			//gLocalVariables.erase( name );
			gLocalVariables.erase( std::string( arg1, eq - arg1 ) );
		}
		else
		{
			// $ export foo
			const char* var = arg1;
			
			if ( getenv( var ) == NULL )
			{
				// Environment variable unset
				StringMap::const_iterator found = gLocalVariables.find( var );
				
				if ( found != gLocalVariables.end() )
				{
					// Shell variable is set, export it
					setenv( var, found->second.c_str(), 1 );
					gLocalVariables.erase( var );
				}
				else
				{
					// Shell variable unset, mark exported
					gVariablesToExport.insert( var );
				}
			}
		}
	}
	
	return 0;
}

static int Builtin_PWD( int /*argc*/, char const* const /*argv*/[] )
{
	std::string cwd;
	cwd.resize( 256 );
	
	while ( getcwd( &cwd[ 0 ], cwd.size() ) == NULL )
	{
		cwd.resize( cwd.size() * 2 );
	}
	
	std::printf( "%s\n", cwd.c_str() );
	
	return 0;
}

static int Builtin_Set( int argc, char const* const argv[] )
{
	if ( argc == 1 )
	{
		// $ set
		std::for_each( gLocalVariables.begin(),
		               gLocalVariables.end(),
		               std::ptr_fun( PrintVariable ) );
	}
	else if ( argc == 2 )
	{
		if ( argv[1] == std::string( "-e" ) )
		{
			SetWhetherToExitOnBatchError( true );
		}
		else if ( argv[1] == std::string( "+e" ) )
		{
			SetWhetherToExitOnBatchError( false );
		}
	}
	else if ( argc == 3 )
	{
		if ( argv[1] == std::string( "-o" ) )
		{
			SetOption( argv[2], true );
		}
		else if ( argv[1] == std::string( "+o" ) )
		{
			SetOption( argv[2], false );
		}
	}
	
	return 0;
}

static int Builtin_Unalias( int argc, char const* const argv[] )
{
	while ( --argc )
	{
		gAliases.erase( argv[ argc ] );
	}
	
	return 0;
}

static int Builtin_Unset( int argc, char const* const argv[] )
{
	while ( --argc )
	{
		gLocalVariables.erase( argv[ argc ] );
		unsetenv( argv[ argc ] );
	}
	
	return 0;
}

class ReplacedParametersScope
{
	private:
		std::size_t         itsSavedParameterCount;
		char const* const*  itsSavedParameters;
	
	public:
		ReplacedParametersScope( std::size_t count, char const* const* params )
		:
			itsSavedParameterCount( gParameterCount ),
			itsSavedParameters    ( gParameters     )
		{
			gParameterCount = count;
			gParameters     = params;
		}
		
		~ReplacedParametersScope()
		{
			gParameterCount = itsSavedParameterCount;
			gParameters     = itsSavedParameters;
		}
};

static int BuiltinDot( int argc, char const* const argv[] )
{
	if ( argc < 2 )
	{
		std::fprintf( stderr, "%s\n", "sh: .: filename argument required" );
		return 2;
	}
	
	NN::Owned< P7::FileDescriptor > fd = P7::Open( argv[ 1 ], 0 );
	
	int controlled = fcntl( fd, F_SETFD, FD_CLOEXEC );
	
	ReplacedParametersScope dotParams( argc - 2, argv + 2 );
	
	int result = ReadExecuteLoop( fd, false );
	
	return result;
}

typedef std::map< std::string, Builtin > BuiltinMap;

static BuiltinMap MakeBuiltins()
{
	BuiltinMap builtins;
	
	builtins[ "alias"   ] = Builtin_Alias;
	builtins[ "cd"      ] = Builtin_CD;
	builtins[ "echo"    ] = Builtin_Echo;
	builtins[ "exit"    ] = Builtin_Exit;
	builtins[ "export"  ] = Builtin_Export;
	builtins[ "pwd"     ] = Builtin_PWD;
	builtins[ "set"     ] = Builtin_Set;
	builtins[ "unalias" ] = Builtin_Unalias;
	builtins[ "unset"   ] = Builtin_Unset;
	builtins[ "."       ] = BuiltinDot;
	
	return builtins;
}

Builtin FindBuiltin( const std::string& name )
{
	static BuiltinMap sBuiltins = MakeBuiltins();
	
	BuiltinMap::const_iterator found = sBuiltins.find( name );
	
	if ( found != sBuiltins.end() )
	{
		return found->second;
	}
	else
	{
		return NULL;
	}
}

