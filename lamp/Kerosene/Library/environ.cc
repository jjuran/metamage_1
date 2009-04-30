/*	==========
 *	environ.cc
 *	==========
 */

#include "environ.hh"

// Mac OS Universal Interfaces
#include <LowMem.h>

// Standard C
#include "errno.h"
#include "stdlib.h"

// Kerosene
#include "environ_store.hh"


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
	try
	{
		get_envp().set( name, value, overwriting );
	}
	catch ( ... )
	{
		errno = ENOMEM;
		
		return -1;
	}
	
	return 0;
}

int putenv( char* string )
{
	try
	{
		get_envp().put( string );
	}
	catch ( ... )
	{
		errno = ENOMEM;
		
		return -1;
	}
	
	return 0;
}

int unsetenv( const char* name )
{
	try
	{
		get_envp().unset( name );
	}
	catch ( ... )
	{
		errno = ENOMEM;
		
		return -1;
	}
	
	return 0;
}

int clearenv()
{
	try
	{
		get_envp().clear();
	}
	catch ( ... )
	{
		errno = ENOMEM;
		
		return -1;
	}
	
	return 0;
}

