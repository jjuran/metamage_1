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

// Standard C/C++
#include <cstring>

// Standard C++
#include <string>

// POSIX
#include "dirent.h"
#include "fcntl.h"
#include "sys/stat.h"
#include "unistd.h"

// Kerosene
#include "environ_store.hh"


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
	
	
	inline iota::environ_t GetEnvironFromKernel()
	{
		return reinterpret_cast< iota::environ_t* >( LMGetToolScratch() )[1];
	}
	
	
	using kerosene::environ_store;
	
	static environ_store *global_environ_top = NULL;
	
	static int global_environ_level = 0;
	static int global_vfork_level   = 0;
	
	static environ_store& get_envp()
	{
		while ( global_vfork_level >= global_environ_level )
		{
			global_environ_top = new environ_store( global_environ_top, environ );
			
			++global_environ_level;
		}
		
		return *global_environ_top;
	}
	
	extern "C" const void* InitializeEnviron();
	
	extern "C" void vfork_push();
	extern "C" void vfork_pop();
	
	const void* InitializeEnviron()
	{
		try
		{
			static environ_store gEnviron( NULL, GetEnvironFromKernel() );
			
			global_environ_top = &gEnviron;
			
			global_environ_level = 1;
		}
		catch ( ... )
		{
		}
		
		return global_environ_top;  // NULL if bad_alloc
	}
	
	void vfork_push()
	{
		++global_vfork_level;
	}
	
	void vfork_pop()
	{
		if ( global_environ_level > global_vfork_level-- )
		{
			global_environ_top = environ_store::pop( global_environ_top );
			
			--global_environ_level;
		}
	}
	
	
	char* getenv( const char* name )
	{
		return global_environ_top->get( name );
	}
	
	int setenv( const char* name, const char* value, int overwriting )
	{
		get_envp().set( name, value, overwriting );
		
		return 0;
	}
	
	int putenv( char* string )
	{
		get_envp().put( string );
		
		return 0;
	}
	
	void unsetenv( const char* name )
	{
		get_envp().unset( name );
	}
	
	int clearenv()
	{
		get_envp().clear();
		
		return 0;
	}
	
	
	DIR* fdopendir( int fd )
	{
		DIR* result = NULL;
		
		if ( fd < 0 )
		{
			errno = EBADF;
			
			return result;
		}
		
		try
		{
			result = new DIR;
			
			result->fd = fd;
		}
		catch ( ... )
		{
			errno = ENOMEM;
		}
		
		return result;
	}
	
	DIR* opendir( const char* pathname )
	{
		DIR* result = NULL;
		
		try
		{
			DIR* dir = new DIR;
			
			int fd = open( pathname, O_RDONLY | O_DIRECTORY );
			
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
			errno = ENOMEM;
		}
		
		return result;
	}
	
	struct dirent* readdir( DIR* dir )
	{
		struct dirent *const entry = &dir->entry;
		
		int got = getdents( dirfd( dir ), entry, sizeof (dirent) );
		
		if ( got <= 0 )
		{
			return NULL;
		}
		
		return entry;
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

