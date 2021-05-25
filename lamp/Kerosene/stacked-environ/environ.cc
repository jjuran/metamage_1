/*
	environ.cc
	----------
*/

// Standard C
#include "stdlib.h"

// Kerosene
#include "environ_store.hh"

// getenv
static char* _getenv( const char* name );
#define getenv _getenv
#include "getenv.cc.hh"
#undef getenv


#pragma exceptions off


using namespace _relix_libc;

static bool loaded_environ;

static inline
bool loadenv()
{
	return loaded_environ = load_environ();
}


char* getenv( const char* name )
{
	return loaded_environ ? environ_get( name )
	                      : _getenv( name );
}

int setenv( const char* name, const char* value, int overwriting )
{
	return (loadenv()  &&  environ_set( name, value, overwriting )) - 1;
}

int putenv( char* string )
{
	return (loadenv()  &&  environ_put( string )) - 1;
}

int unsetenv( const char* name )
{
	return (loadenv()  &&  (environ_unset( name ), true)) - 1;
}

int clearenv()
{
	return (loadenv()  &&  (environ_clear(), true )) - 1;
}
