/*
	syscalls.c
	----------
*/


extern int syscall( int number, ... );

static void* global_dispatcher;

extern void _set_dispatcher( void* address );

void _set_dispatcher( void* address )
{
	global_dispatcher = address;
}

#if defined( __MC68K__ )  ||  defined( __SC__ )

	#include "syscalls.68k.c.h"
	
#endif

#ifdef __POWERPC__
	
	#include "syscalls.ppc.c.h"
	
#endif

// syscalls
#include "define_syscalls.h"
