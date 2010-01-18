/*
	readonly-environ.cc
	-------------------
	
	Copyright 2010, Joshua Juran
*/

// Standard C
#include "errno.h"
#include "stdlib.h"

// Lamp
#include "lamp/environ_stack.h"


extern "C" char **environ;

char **environ = NULL;


extern "C" const void* _initialize_environ( char **envp );

const void* _initialize_environ( char **envp )
{
	return environ = envp;
}

void _push_environ()
{
}

void _pop_environ()
{
}


char* getenv( const char* name )
{
	if ( environ != NULL )
	{
		for ( char** env = environ;  *env != NULL;  ++env )
		{
			const char* var = *env;
			
			if ( char* eq = strchr( var, '=' ) )
			{
				const size_t name_length = eq - var;
				
				if ( memcmp( name, var, name_length ) == 0  &&  name[ name_length ] == '\0' )
				{
					return eq + 1;
				}
			}
		}
	}
	
	return NULL;
}

int setenv( const char* name, const char* value, int overwriting )
{
	errno = ENOSYS;
	
	return -1;
}

int putenv( char* string )
{
	errno = ENOSYS;
	
	return -1;
}

int unsetenv( const char* name )
{
	errno = ENOSYS;
	
	return -1;
}

int clearenv()
{
	errno = ENOSYS;
	
	return -1;
}

