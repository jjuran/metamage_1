/*
	readonly-environ.cc
	-------------------
	
	Copyright 2010, Joshua Juran
*/

// Standard C
#include "stdlib.h"

// getenv
#include "getenv.cc.hh"


extern "C" char **environ;

char **environ = NULL;


extern "C" int mutable_environment_is_unimplemented();


int setenv( const char* name, const char* value, int overwriting )
{
	return mutable_environment_is_unimplemented();
}

int putenv( char* string )
{
	return mutable_environment_is_unimplemented();
}

int unsetenv( const char* name )
{
	return mutable_environment_is_unimplemented();
}

int clearenv()
{
	return mutable_environment_is_unimplemented();
}

