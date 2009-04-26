/*	==================
 *	LibraryThrowing.cc
 *	==================
 */

// Mac OS Universal Interfaces
#include <LowMem.h>

// Standard C
#include "errno.h"
#include "stdlib.h"

// POSIX
#include "dirent.h"
#include "fcntl.h"
#include "unistd.h"

// Kerosene
#include "environ_store.hh"


//

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

