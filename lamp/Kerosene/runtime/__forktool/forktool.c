/*
	forktool.c
	----------
*/

// Standard C
#include <stddef.h>

// Relix
#include "relix/parameter_block.h"
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

typedef _relix_system_parameter_block system_pb;

// Call main() and exit()
extern void _relix_main( int argc, char** argv, char** envp, system_pb* pb );
extern int         main( int argc, char** argv );

extern void exit( int );


void _relix_main( int argc, char** argv, char** envp, system_pb* pb )
{
	const size_t min_size = offsetof( system_pb, runctl ) + sizeof (long (*)());
	
	_set_dispatcher( pb->dispatcher );
	
	environ = envp;
	
	global_system_params = pb;
	global_user_params   = pb->current_user;
	
	global_user_params->errno_var = &errno;
	global_user_params->cleanup   = &_MSL_cleanup;
	
#ifdef __MC68K__
	
	global_user_params->globals = GetA4();
	
#endif
	
	INITIALIZE();
	
	if ( pb->system_block_length >= min_size  &&  pb->runctl != NULL )
	{
		pb->runctl( runctl_allocate_syscall_stack );
	}
	
	exit( main( argc, argv ) );
}
