// ===========
// Builtins.cc
// ===========

#include "Builtins.hh"

// Standard C/C++
#include <cstring>

// Standard C
#include "stdlib.h"

// POSIX
#include "fcntl.h"
#include "unistd.h"

// Io
#include "Io/MakeHandle.hh"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"
#include "SystemCalls.hh"

// sh
#include "ReadExecuteLoop.hh"


namespace O = Orion;


static StringMap gLocalVariables;
static StringMap gAliases;

static Io::Stream< Io::FD > In, Out, Err;

void SetInputFD( int fd )
{
	In = Io::FD( fd );
}

void SetOutputFD( int fd )
{
	Out = Io::FD( fd );
}

void SetErrorFD( int fd )
{
	Err = Io::FD( fd );
}

static void PrintVariable( const StringMap::value_type& var )
{
	const std::string& name  = var.first;
	const std::string& value = var.second;
	
	Out << name << "='" << value << "'\n";
}

static void PrintAlias( const StringMap::value_type& var )
{
	const std::string& name  = var.first;
	const std::string& value = var.second;
	
	Out << "alias " << name << "='" << value << "'\n";
}


int Assign( const char* name, const char* value )
{
	resetenv( name, value );
	
	if ( getenv( name ) )
	{
		gLocalVariables.erase( name );
	}
	else
	{
		gLocalVariables[ name ] = value;
	}
	
	return 0;
}

// Builtins.  argc is guaranteed to be positive.

static int Builtin_CD( int argc, char const* const argv[] )
{
	if ( argc == 1 )
	{
		// cd home
	}
	else
	{
		chdir( argv[ 1 ] );
	}
	
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
		Out << argv[ 1 ];
		
		for ( short i = 2; i < argc; i++ )
		{
			Out << " " << argv[ i ];
		}
	}
	
	Out << "\n";
	
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
			Out << "export " << *envp << "\n";
			++envp;
		}
	}
	else if ( argc == 2 )
	{
		const char* const arg1 = argv[ 1 ];
		
		if ( char* eq = std::strchr( arg1, '=' ) )
		{
			// $ export foo=bar
			
			//*eq = '\0';
			
			//char* name = argv[ 1 ];
			
			//setenv( name, eq + 1, 1 );
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
				// Environment variable unset, or set with no value
				StringMap::const_iterator found = gLocalVariables.find( var );
				
				if ( found != gLocalVariables.end() )
				{
					// Shell variable is set, export it
					setenv( var, found->second.c_str(), 1 );
					gLocalVariables.erase( var );
				}
				else
				{
					// Shell variable unset, export with no value
					setenv( var, NULL, 0 );
				}
			}
		}
	}
	
	return 0;
}

static int Builtin_PS( int argc, char const* const argv[] )
{
	PrintPS();
	
	return 0;
}

static int Builtin_PWD( int argc, char const* const argv[] )
{
	std::string cwd;
	cwd.resize( 256 );
	
	while ( getcwd( &cwd[ 0 ], cwd.size() ) == NULL )
	{
		cwd.resize( cwd.size() * 2 );
	}
	
	Out << cwd.c_str() << "\n";
	
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

static int BuiltinDot( int argc, char const* const argv[] )
{
	if ( argc < 2 )
	{
		Err << "sh: .: filename argument required\n";
		return 2;
	}
	
	std::vector< const char* > positionalParameters( argc - 1 );
	
	std::copy( argv + 1,
	           argv + argc,
	           positionalParameters.begin() );
	
	int fd = open( argv[ 1 ], 0 );
	
	Io::Handle in = Io::MakeHandleFromCast< Io::FD_Details, Io::FD >( Io::FD( fd ) );
	
	int result = ReadExecuteLoop( in, positionalParameters );
	
	(void)close( fd );
	
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
	builtins[ "ps"      ] = Builtin_PS;
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

