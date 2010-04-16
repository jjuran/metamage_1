// ===========
// Builtins.cc
// ===========

#include "Builtins.hh"

// Standard C++
#include <set>

// Standard C/C++
#include <cstdio>
#include <cstring>

// Standard C
#include <stdlib.h>

// POSIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

// Iota
#include "iota/decimal.hh"
#include "iota/environ.hh"

// poseven
#include "poseven/functions/fcntl.hh"
#include "poseven/functions/open.hh"

// sh
#include "Options.hh"
#include "PositionalParameters.hh"
#include "Execution.hh"
#include "ReadExecuteLoop.hh"


namespace tool
{
	
	namespace n = nucleus;
	namespace p7 = poseven;
	
	
	typedef std::map< std::string, std::string > StringMap;
	
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
	
	void AssignShellVariable( const char* name, const char* value )
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
	
	static p7::exit_t Builtin_CD( int argc, iota::argv_t argv )
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
		
		int changed = chdir( dir );
		
		if ( changed < 0 )
		{
			std::fprintf( stderr, "cd: %s: %s\n", dir, std::strerror( errno ) );
			
			return p7::exit_failure;
		}
		
		// Apparently setenv() breaks something.
		//setenv( "OLDPWD", getenv( "PWD" ), 1 );
		
		// FIXME:  This should be full pathname.
		//setenv( "PWD", argv[ 1 ], 1 );
		
		return p7::exit_success;
	}
	
	static p7::exit_t Builtin_Alias( int argc, iota::argv_t argv )
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
		
		return p7::exit_success;
	}
	
	static p7::exit_t Builtin_Echo( int argc, iota::argv_t argv )
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
		
		return p7::exit_success;
	}
	
	static p7::exit_t Builtin_Exit( int argc, iota::argv_t argv )
	{
		int exitStatus = 0;
		
		if ( argc > 1 )
		{
			exitStatus = iota::parse_unsigned_decimal( argv[ 1 ] );
		}
		
		throw p7::exit_t( exitStatus );
		
		// Not reached
		return p7::exit_success;
	}
	
	static p7::exit_t Builtin_Export( int argc, iota::argv_t argv )
	{
		if ( argc == 1 )
		{
			// $ export
			iota::envp_t envp = environ;
			
			while ( *envp != NULL )
			{
				std::printf( "export %s\n", *envp );
				++envp;
			}
		}
		else if ( argc == 2 )
		{
			iota::arg_t const arg1 = argv[ 1 ];
			
			if ( const char* eq = std::strchr( arg1, '=' ) )
			{
				// $ export foo=bar
				std::string name( arg1, eq - arg1 );
				
				setenv( name.c_str(), eq + 1, true );
				
				gLocalVariables.erase( name );
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
		
		return p7::exit_success;
	}
	
	static p7::exit_t Builtin_PWD( int /*argc*/, iota::argv_t /*argv*/ )
	{
		std::string cwd;
		cwd.resize( 256 );
		
		while ( getcwd( &cwd[ 0 ], cwd.size() ) == NULL )
		{
			cwd.resize( cwd.size() * 2 );
		}
		
		std::printf( "%s\n", cwd.c_str() );
		
		return p7::exit_success;
	}
	
	static p7::exit_t Builtin_Set( int argc, iota::argv_t argv )
	{
		if ( argc == 1 )
		{
			// $ set
			std::for_each( gLocalVariables.begin(),
			               gLocalVariables.end(),
			               std::ptr_fun( PrintVariable ) );
		}
		else
		{
			const char* first = argv[1];
			
			if ( first[0] == '-'  ||  first[0] == '+' )
			{
				const bool value = first[0] == '-';
				
				if ( argc == 2  &&  first[1] == 'e' )
				{
					SetOption( kOptionExitOnError, value );
				}
				else if ( argc == 3  &&  first[1] == 'o' )
				{
					SetOptionByName( argv[2], value );
				}
			}
		}
		
		return p7::exit_success;
	}
	
	static p7::exit_t Builtin_Unalias( int argc, iota::argv_t argv )
	{
		while ( --argc )
		{
			gAliases.erase( argv[ argc ] );
		}
		
		return p7::exit_success;
	}
	
	static p7::exit_t Builtin_Unset( int argc, iota::argv_t argv )
	{
		while ( --argc )
		{
			gLocalVariables.erase( argv[ argc ] );
			unsetenv( argv[ argc ] );
		}
		
		return p7::exit_success;
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
	
	static p7::exit_t BuiltinDot( int argc, iota::argv_t argv )
	{
		if ( argc < 2 )
		{
			std::fprintf( stderr, "%s\n", "sh: .: filename argument required" );
			
			return p7::exit_t( 2 );
		}
		
	#ifdef O_CLOEXEC
		
		const p7::open_flags_t flags = p7::o_rdonly | p7::o_cloexec;
		
	#else
		
		const p7::open_flags_t flags = p7::o_rdonly;
		
	#endif
		
		n::owned< p7::fd_t > fd = p7::open( argv[ 1 ], flags );
		
	#ifndef O_CLOEXEC
		
		p7::fcntl< p7::f_setfd >( fd, p7::fd_cloexec );
		
	#endif
		
		ReplacedParametersScope dotParams( argc - 2, argv + 2 );
		
		return n::convert< p7::exit_t >( ReadExecuteLoop( fd, false ) );
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
	
}

