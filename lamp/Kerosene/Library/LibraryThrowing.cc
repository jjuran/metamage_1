/*	==================
 *	LibraryThrowing.cc
 *	==================
 */

// Mac OS Universal Interfaces
#include <LowMem.h>

// Standard C
#include "errno.h"
#include <stdarg.h>
#include "stdlib.h"

// Standard C++
#include <string>
#include <vector>

// POSIX
#include "dirent.h"
#include "fcntl.h"
#include "sys/stat.h"
#include "unistd.h"

// Iota
#include "iota/environ.hh"

// ShellShock
#include "ShellShock/VarArray.hh"


//

	static std::string LookupPath( const char* filename )
	{
		const char* pathVar = getenv( "PATH" );
		
		if ( pathVar == NULL )
		{
			return "";
		}
		
		const char* pathEnd = pathVar + std::strlen( pathVar );
		
		while ( pathVar < pathEnd )
		{
			const char* separator = std::strchr( pathVar, ':' );
			
			if ( separator == NULL )
			{
				separator = pathEnd;
			}
			
			// Watch out for empty path elements (e.g. "/bin:/sbin:" -- last is empty)
			if ( separator != pathVar )
			{
				std::string dir( pathVar, separator - pathVar );
				
				std::string tryPath = dir + "/" + filename;
				
				struct ::stat sb;
				int status = stat( tryPath.c_str(), &sb );
				
				if ( status == 0  &&  sb.st_mode & S_IXUSR )
				{
					return tryPath;
				}
			}
			
			// If we're at the end, then this sets pathVar > pathEnd
			pathVar = separator + 1;
		}
		
		return "";
	}
	
	int execl( const char* path, const char* arg0, ... )
	{
		va_list va;
		std::vector< const char* > args;
		
		args.push_back( arg0 );
		
		va_start( va, arg0 );
		
		while ( const char* arg = va_arg( va, const char * ) )
		{
			args.push_back( arg );
		}
		
		va_end( va );
		
		args.push_back( NULL );
		
		const char* const* argv = &args[0];
		
		return execv( path, argv );
	}
	
	int execle( const char* path, const char* arg0, ... )
	{
		va_list va;
		std::vector< const char* > args;
		
		args.push_back( arg0 );
		
		va_start( va, arg0 );
		
		while ( const char* arg = va_arg( va, const char * ) )
		{
			args.push_back( arg );
		}
		
		args.push_back( NULL );
		
		const char* const* envp = va_arg( va, const char* const* );
		
		va_end( va );
		
		const char* const* argv = &args[0];
		
		return execve( path, argv, envp );
	}
	
	int execlp( const char* file, const char* arg0, ... )
	{
		
		va_list va;
		std::vector< const char* > args;
		
		args.push_back( arg0 );
		
		va_start( va, arg0 );
		
		while ( const char* arg = va_arg( va, const char * ) )
		{
			args.push_back( arg );
		}
		
		va_end( va );
		
		args.push_back( NULL );
		
		const char* const* argv = &args[0];
		
		return execvp( file, argv );
	}
	
	int execvp( const char* file, const char* const argv[] )
	{
		std::string path;
		
		if ( std::strchr( file, '/' ) == NULL )
		{
			path = LookupPath( file );
			
			if ( path == "" )
			{
				errno = ENOENT;
				
				return -1;
			}
			
			file = path.c_str();
		}
		
		return execv( file, argv );
	}
	
	
	iota::environ_t environ = NULL;
	
	
	inline iota::environ_t GetEnvironFromKernel()
	{
		return reinterpret_cast< iota::environ_t* >( LMGetToolScratch() )[1];
	}
	
	inline char* copy_string( const char* s )
	{
		std::size_t len = std::strlen( s );
		
		char* result = new char[ len + 1 ];
		
		std::copy( s, s + len + 1, result );
		
		return result;
	}
	
	inline char* copy_string( const std::string& s )
	{
		char* result = new char[ s.size() + 1 ];
		
		std::copy( s.c_str(), s.c_str() + s.size() + 1, result );
		
		return result;
	}
	
	static void DeleteVars( std::vector< char* >& result )
	{
		for ( int i = result.size() - 1;  i >= 0;  --i )
		{
			delete [] result[ i ];
		}
	}
	
	static void CopyVars( char const *const *vars, std::vector< char* >& result  )
	{
		try
		{
			if ( vars != NULL )
			{
				while ( *vars )
				{
					// This ensures push_back() won't (fail to) allocate memory
					result.reserve( result.size() + 1 );
					
					result.push_back( copy_string( *vars++ ) );
				}
			}
			
			result.push_back( NULL );
		}
		catch ( ... )
		{
			DeleteVars( result );
			
			throw;
		}
	}
	
	
	namespace Sh = ShellShock;
	
	static std::vector< char* >::iterator FindVar( std::vector< char* >& vars, const char* name )
	{
		return std::lower_bound( vars.begin(),
		                         vars.end() - 1,
		                         name,
		                         std::ptr_fun( Sh::CompareStrings ) );
	}
	
	static void OverwriteVar( std::vector< char* >::iterator it, const std::string& string )
	{
		char* var = *it;
		
		std::size_t old_len = std::strlen( var );
		
		std::size_t new_len = string.size();
		
		if ( new_len != old_len )
		{
			var = new char[ new_len + 1 ];
			
			delete [] *it;
			
			*it = var;
		}
		
		std::copy( string.c_str(), string.c_str() + new_len + 1, var );
	}
	
	
	class Environ
	{
		private:
			std::vector< char* > itsVars;
		
		public:
			Environ();
			
			~Environ();
			
			void UpdateEnvironValue();
			
			char* GetEnv( const char* name );
			void SetEnv( const char* name, const char* value, bool overwrite );
			void PutEnv( const char* string );
			void UnsetEnv( const char* name );
			void ClearEnv();
	};
	
	
	Environ::Environ()
	{
		CopyVars( GetEnvironFromKernel(), itsVars );
		
		UpdateEnvironValue();
	}
	
	Environ::~Environ()
	{
		DeleteVars( itsVars );
	}
	
	void Environ::UpdateEnvironValue()
	{
		environ = &itsVars.front();
	}
	
	char* Environ::GetEnv( const char* name )
	{
		std::vector< char* >::iterator it = FindVar( itsVars, name );
		
		char* var = *it;
		
		const char* end = Sh::EndOfVarName( var );
		
		// Did we find the right environment variable?
		if ( end != NULL  &&  *end == '='  &&  Sh::VarMatchesName( var, end, name ) )
		{
			return var + (end - var) + 1;
		}
		
		return NULL;
	}
	
	void Environ::SetEnv( const char* name, const char* value, bool overwrite )
	{
		std::vector< char* >::iterator it = FindVar( itsVars, name );
		
		const char* var = *it;
		
		// Did we find the right environment variable?
		bool match = Sh::VarMatchesName( var, Sh::EndOfVarName( var ), name );
		
		// If it doesn't match, we insert (otherwise, we possibly overwrite)
		bool inserting = !match;
		
		if ( inserting )
		{
			itsVars.insert( it, copy_string( Sh::MakeVar( name, value ) ) );
			
			UpdateEnvironValue();
		}
		else if ( overwrite )
		{
			OverwriteVar( it, Sh::MakeVar( name, value ) );
		}
	}
	
	void Environ::PutEnv( const char* string )
	{
		std::string name = string;
		name.resize( name.find( '=' ) );
		
		std::vector< char* >::iterator it = FindVar( itsVars, name.c_str() );
		
		const char* var = *it;
		
		// Did we find the right environment variable?
		bool match = Sh::VarMatchesName( var, Sh::EndOfVarName( var ), name.c_str() );
		
		// If it doesn't match, we insert (otherwise, we possibly overwrite)
		bool inserting = !match;
		
		if ( inserting )
		{
			itsVars.insert( it, copy_string( string ) );
			
			UpdateEnvironValue();
		}
		else
		{
			OverwriteVar( it, string );
		}
	}
	
	void Environ::UnsetEnv( const char* name )
	{
		std::vector< char* >::iterator it = FindVar( itsVars, name );
		
		const char* var = *it;
		
		// Did we find the right environment variable?
		bool match = Sh::VarMatchesName( var, Sh::EndOfVarName( var ), name );
		
		if ( match )
		{
			itsVars.erase( it );
			
			delete [] var;
		}
	}
	
	void Environ::ClearEnv()
	{
		DeleteVars( itsVars );
		
		environ = NULL;
	}
	
	static Environ gEnviron;
	
	
	char* getenv( const char* name )
	{
		return gEnviron.GetEnv( name );
	}
	
	int setenv( const char* name, const char* value, int overwrite )
	{
		gEnviron.SetEnv( name, value, overwrite );
		
		return 0;
	}
	
	int putenv( const char* string )
	{
		gEnviron.PutEnv( string );
		
		return 0;
	}
	
	void unsetenv( const char* name )
	{
		gEnviron.UnsetEnv( name );
	}
	
	extern "C" int clearenv();
	
	int clearenv()
	{
		gEnviron.ClearEnv();
		
		return 0;
	}
	
	
	DIR* opendir( const char* pathname )
	{
		DIR* result = NULL;
		
		try
		{
			DIR* dir = new DIR;
			
			int fd = open( pathname, O_RDONLY, 0 );
			
			if ( fd == -1 )
			{
				delete dir;
			}
			else
			{
				dir->fd = fd;
				
				result = dir;
			}
		}
		catch ( ... )
		{
		}
		
		return result;
	}
	
	struct dirent* readdir( DIR* dir )
	{
		static dirent entry;
		
		int got = getdents( dirfd( dir ), &entry, sizeof (dirent) );
		
		if ( got <= 0 )
		{
			return NULL;
		}
		
		return &entry;
	}
	
	int closedir( DIR* dir )
	{
		int fd = dirfd( dir );
		
		delete dir;
		
		return close( fd );
	}
	
	void rewinddir( DIR* dir )
	{
		(void) lseek( dirfd( dir ), 0, SEEK_SET );
	}
	
	long telldir( DIR* dir )
	{
		return lseek( dirfd( dir ), 0, SEEK_CUR );
	}
	
	void seekdir( DIR* dir, long offset )
	{
		(void) lseek( dirfd( dir ), offset, SEEK_SET );
	}
	
	int dirfd( DIR* dir )
	{
		return dir->fd;
	}
	
//

