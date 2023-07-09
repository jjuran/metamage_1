/*
	microtool.c
	-----------
	
	Joshua Juran
*/

// Relix
#include "relix/parameter_block.h"
#include "tool-runtime/parameter_block.h"


typedef _relix_system_parameter_block system_pb;

#ifdef __MC68K__

static inline
asm
void* GetA4()
{
	MOVEA.L A4,A0
}

#endif

extern void _set_dispatcher( void *address );

extern char** environ;

extern int errno;

// Call main() and return
extern int _relix_main( int argc, char **argv, char **envp, system_pb* pb );
extern int        main( int argc, char **argv );


int _relix_main( int argc, char **argv, char **envp, system_pb* pb )
{
	_set_dispatcher( pb->dispatcher );
	
	environ = envp;
	
	global_system_params = pb;
	global_user_params   = pb->current_user;
	
	global_user_params->errno_var = &errno;
//	global_user_params->cleanup   = NULL;
	
#ifdef __MC68K__
	
	global_user_params->globals = GetA4();
	
#endif
	
	return main( argc, argv );
}
