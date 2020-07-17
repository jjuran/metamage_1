// ===========
// Builtins.cc
// ===========

#include "Builtins.hh"

// Standard C++
#include <algorithm>
#include <map>
#include <set>

// Standard C/C++
#include <cstring>

// Standard C
#include <stdlib.h>

// POSIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

// Iota
#include "iota/strings.hh"

// gear
#include "gear/parse_decimal.hh"

// more-posix
#include "more/perror.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/functions/fcntl.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/write.hh"

// sh
#include "Options.hh"
#include "PositionalParameters.hh"
#include "Execution.hh"
#include "ReadExecuteLoop.hh"


extern "C" char** environ;


namespace tool
{
	
	namespace n = nucleus;
	namespace p7 = poseven;
	
	
	typedef std::map< plus::string, plus::string > StringMap;
	
	static StringMap gLocalVariables;
	static StringMap gAliases;
	
	static std::set< plus::string > gVariablesToExport;
	
	static void PrintVariable( const StringMap::value_type& var )
	{
		const plus::string& name  = var.first;
		const plus::string& value = var.second;
		
		plus::var_string variable = name;
		
		variable += "='";
		variable +=    value;
		variable +=        "'\n";
		
		p7::write( p7::stdout_fileno, variable );
	}
	
	static void PrintAlias( const StringMap::value_type& var )
	{
		const plus::string& name  = var.first;
		const plus::string& value = var.second;
		
		plus::var_string alias = "alias ";
		
		alias += name;
		alias +=    "='";
		alias +=       value;
		alias +=           "'\n";
		
		p7::write( p7::stdout_fileno, alias );
	}
	
	
	static bool UnmarkVariableForExport( const plus::string& name )
	{
		std::set< plus::string >::iterator found = gVariablesToExport.find( name );
		
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
	
	const char* QueryShellVariable( const plus::string& name )
	{
		StringMap::const_iterator found = gLocalVariables.find( name );
		
		if ( found != gLocalVariables.end() )
		{
			return found->second.c_str();
		}
		
		return NULL;
	}
	
	static const char* get_home()
	{
		if ( const char* home = getenv( "HOME" ) )
		{
			return home;
		}
		
		return "/";
	}
	
#ifndef __RELIX__
	
	static inline ssize_t _getcwd( char* buffer, size_t length )
	{
		if ( const char* gotcwd = getcwd( buffer, length ) )
		{
			return strlen( gotcwd );
		}
		
		return -1;
	}
	
#endif
	
	static plus::string get_cwd()
	{
		plus::var_string cwd;
		
		const size_t buffer_size = 4096;
		
		ssize_t got = _getcwd( cwd.reset( buffer_size ), buffer_size );
		
		p7::throw_posix_result( got );
		
		cwd.resize( got );
		
		return cwd;
	}
	
	static plus::string change_pwd( const char* old_pwd, const char* path )
	{
		plus::var_string new_pwd;
		
		if ( path[0] != '/' )
		{
			new_pwd = old_pwd;
		}
		
		const char* end = path + strlen( path );
		
		const char* p = path;
		
		while ( p < end )
		{
			const char* slash = std::find( p, end, '/' );
			
			const size_t length = slash - p;
			
			if ( length == 0 )
			{
				++p;
			}
			else
			{
				if ( p[0] == '.' )
				{
					switch ( length )
					{
						case 2:
							if ( p[1] != '.' )
							{
								break;
							}
							
							if ( !new_pwd.empty() )
							{
								new_pwd.resize( new_pwd.find_last_of( "/" ) );
							}
						
						case 1:
							p += length;
							
							continue;
						
						default:
							break;
					}
				}
				
				new_pwd += '/';
				new_pwd.append( p, length );
				
				p += length;
			}
		}
		
		if ( new_pwd.empty() )
		{
			new_pwd = '/';
		}
		
		return new_pwd;
	}
	
	static bool get_physical( char**& args )
	{
		bool physical = false;
		
		while ( *++args != NULL )
		{
			const char* arg = *args;
			
			if ( arg[0] == '-' )
			{
				if ( arg[1] == 'P' )
				{
					physical = true;
				}
				
				if ( arg[1] == 'L' )
				{
					physical = false;
				}
			}
			else
			{
				break;
			}
		}
		
		return physical;
	}
	
	// Builtins.  argc is guaranteed to be positive.
	
	static p7::exit_t Builtin_CD( int argc, char** argv )
	{
		char** args = argv;
		
		const bool physical = get_physical( args );
		
		const char* dir = *args ? *args : get_home();
		
		int changed = chdir( dir );
		
		if ( changed < 0 )
		{
			more::perror( "cd", dir );
			
			return p7::exit_failure;
		}
		
		const char* old_pwd = getenv( "PWD" );
		
		if ( old_pwd != NULL )
		{
			if ( old_pwd[0] != '/' )
			{
				old_pwd = NULL;
			}
			else
			{
				setenv( "OLDPWD", old_pwd, 1 );
			}
		}
		
		const bool use_cwd = physical  ||  old_pwd == NULL;
		
		plus::string new_pwd = use_cwd ? get_cwd() : change_pwd( old_pwd, dir );
		
		setenv( "PWD", new_pwd.c_str(), 1 );
		
		return p7::exit_success;
	}
	
	static p7::exit_t Builtin_Alias( int argc, char** argv )
	{
		if ( argc == 1 )
		{
			// $ alias
			
			typedef StringMap::const_iterator Iter;
			
			Iter end = gAliases.end();
			
			for ( Iter it = gAliases.begin();  it != end;  ++it )
			{
				PrintAlias( *it );
			}
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
	
	static p7::exit_t Builtin_Echo( int argc, char** argv )
	{
		plus::var_string line;
		
		if ( argc > 1 )
		{
			line = argv[1];
			
			for ( short i = 2; i < argc; i++ )
			{
				line += " ";
				line += argv[i];
			}
		}
		
		line += '\n';
		
		p7::write( p7::stdout_fileno, line );
		
		return p7::exit_success;
	}
	
	static p7::exit_t Builtin_Exec( int argc, char** argv )
	{
		p7::exit_t exit_status = p7::exit_success;
		
		if ( argc > 1 )
		{
			(void) execvp( argv[1], argv + 1 );
			
			exit_status = p7::exit_t( errno == ENOENT ? 127 : 126 );
			
			more::perror( "exec", argv[1] );
		}
		
		return exit_status;
	}
	
	static p7::exit_t Builtin_Exit( int argc, char** argv )
	{
		int exitStatus = 0;
		
		if ( argc > 1 )
		{
			exitStatus = gear::parse_unsigned_decimal( argv[ 1 ] );
		}
		
		throw p7::exit_t( exitStatus );
		
		// Not reached
		return p7::exit_success;
	}
	
	static p7::exit_t Builtin_Export( int argc, char** argv )
	{
		if ( argc == 1 )
		{
			// $ export
			char** envp = environ;
			
			while ( *envp != NULL )
			{
				// Can't use the name 'export' in Metrowerks C++
				plus::var_string export_ = "export ";
				
				export_ += *envp;
				export_ += '\n';
				
				p7::write( p7::stdout_fileno, export_ );
				
				++envp;
			}
		}
		else if ( argc == 2 )
		{
			const char* const arg1 = argv[ 1 ];
			
			if ( const char* eq = std::strchr( arg1, '=' ) )
			{
				// $ export foo=bar
				plus::string name( arg1, eq - arg1 );
				
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
	
	static p7::exit_t Builtin_PWD( int argc, char** argv )
	{
		char** args = argv;
		
		const bool physical = get_physical( args );
		
		plus::var_string cwd;
		
		if ( !physical )
		{
			if ( const char* pwd = getenv( "PWD" ) )
			{
				if ( pwd[0] == '/' )
				{
					cwd = pwd;
				}
			}
		}
		
		if ( cwd.empty() )
		{
			char* p = cwd.reset( 256 );
			
			while ( getcwd( p, cwd.size() ) == NULL )
			{
				p = cwd.reset( cwd.size() * 2 );
			}
			
			cwd.resize( strlen( cwd.c_str() ) );
		}
		
		cwd += '\n';
		
		p7::write( p7::stdout_fileno, cwd );
		
		return p7::exit_success;
	}
	
	static p7::exit_t Builtin_Set( int argc, char** argv )
	{
		if ( argc == 1 )
		{
			// $ set
			
			typedef StringMap::const_iterator Iter;
			
			Iter end = gLocalVariables.end();
			
			for ( Iter it = gLocalVariables.begin();  it != end;  ++it )
			{
				PrintVariable( *it );
			}
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
	
	static p7::exit_t Builtin_Unalias( int argc, char** argv )
	{
		while ( --argc )
		{
			gAliases.erase( argv[ argc ] );
		}
		
		return p7::exit_success;
	}
	
	static p7::exit_t Builtin_Unset( int argc, char** argv )
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
	
	static p7::exit_t BuiltinDot( int argc, char** argv )
	{
		if ( argc < 2 )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "sh: .: filename argument required\n" ) );
			
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
	
	struct builtin_t
	{
		const char*    name;
		const Builtin  code;
	};
	
	static inline bool operator==( const builtin_t& b, const plus::string& s )
	{
		return b.name == s;
	}
	
	static const builtin_t builtins[] =
	{
		{ "alias",   Builtin_Alias   },
		{ "cd",      Builtin_CD      },
		{ "echo",    Builtin_Echo    },
		{ "exec",    Builtin_Exec    },
		{ "exit",    Builtin_Exit    },
		{ "export",  Builtin_Export  },
		{ "pwd",     Builtin_PWD     },
		{ "set",     Builtin_Set     },
		{ "unalias", Builtin_Unalias },
		{ "unset",   Builtin_Unset   },
		{ ".",       BuiltinDot      }
	};
	
	Builtin FindBuiltin( const plus::string& name )
	{
		const builtin_t* begin = builtins;
		const builtin_t* end   = begin + sizeof builtins / sizeof builtins[0];
		
		const builtin_t* it = std::find( begin, end, name );
		
		return it != end ? it->code : NULL;
	}
	
}
