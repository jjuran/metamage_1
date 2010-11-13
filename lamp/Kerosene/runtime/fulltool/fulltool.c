/*
	fulltool.c
	----------
*/

// Lamp
#include "lamp/parameter_block.h"
#include "tool-runtime/parameter_block.h"


#ifdef __MC68K__

	static inline asm void* GetA4()
	{
		MOVEA.L A4,A0
	}

#endif

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

extern int errno;

extern void _MSL_cleanup();

// Call main() and exit()
extern void _lamp_main( int argc, char** argv, char** envp, _lamp_system_parameter_block* pb );
extern int        main( int argc, char** argv );

extern void exit( int );


#pragma force_active on

void _lamp_main( int argc, char** argv, char** envp, _lamp_system_parameter_block* pb )
{
#ifndef __MC68K__
	
	_set_dispatcher( pb->dispatcher );
	
#endif
	
	environ = envp;
	
	global_system_params = pb;
	global_user_params   = pb->current_user;
	
	global_user_params->errno_var = &errno;
	global_user_params->cleanup   = &_MSL_cleanup;
	
#ifdef __MC68K__
	
	global_user_params->globals = GetA4();
	
#endif
	
	INITIALIZE();
	
	exit( main( argc, argv ) );
}

#pragma force_active reset

