/*
	fulltool.c
	----------
*/

// Lamp
#include "lamp/parameter_block.h"
#include "tool-runtime/parameter_block.h"

// MSL Runtime
#ifdef __MC68K__
extern void __InitCode__();
#define INITIALIZE()  __InitCode__()
#else
extern pascal short __initialize( const void* initBlock );
#define INITIALIZE()  __initialize( 0 )  /* NULL */
#endif

extern void _set_dispatcher( void* address );

extern char** environ;

// Call InitProc() to set references to cleanup proc and errno
extern void InitializeCallbacks();

// Call main() and exit()
extern void _lamp_main( int argc, char** argv, char** envp, _lamp_system_parameter_block* pb );
extern int        main( int argc, char** argv );

extern void exit( int );


#pragma force_active on

void _lamp_main( int argc, char** argv, char** envp, _lamp_system_parameter_block* pb )
{
	_set_dispatcher( pb->dispatcher );
	
	environ = envp;
	
	global_system_params = pb;
	global_user_params   = pb->current_user;
	
	InitializeCallbacks();
	
	INITIALIZE();
	
	exit( main( argc, argv ) );
}

#pragma force_active reset

