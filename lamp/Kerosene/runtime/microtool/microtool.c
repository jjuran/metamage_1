/*
	microtool.c
	-----------
	
	Joshua Juran
*/

// Lamp
#include "lamp/parameter_block.h"
#include "tool-runtime/parameter_block.h"


extern void _set_dispatcher( void *address );

extern char** environ;

// Call InitProc() to set references to cleanup proc and errno
extern void InitializeCallbacks();

// Call main() and return
extern int _lamp_main( int argc, char **argv, char **envp, _lamp_system_parameter_block* pb );
extern int       main( int argc, char **argv );

#pragma force_active on

int _lamp_main( int argc, char **argv, char **envp, _lamp_system_parameter_block* pb )
{
	_set_dispatcher( pb->dispatcher );
	
	environ = envp;
	
	global_system_params = pb;
	global_user_params   = pb->current_user;
	
	InitializeCallbacks();
	
	return main( argc, argv );
}

#pragma force_active reset

