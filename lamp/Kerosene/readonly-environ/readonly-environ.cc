/*
	readonly-environ.cc
	-------------------
	
	Copyright 2010, Joshua Juran
*/

// Standard C
#include "stdlib.h"

// Lamp
#include "lamp/environ_stack.h"

// getenv
#include "getenv.cc.hh"


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

