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

