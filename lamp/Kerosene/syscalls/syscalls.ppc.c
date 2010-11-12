/*
	syscalls.ppc.c
	--------------
*/


extern int syscall( int number, ... );

static void* global_dispatcher;

extern void _set_dispatcher( void* address );

void _set_dispatcher( void* address )
{
	global_dispatcher = address;
}

#ifdef __POWERPC__
	
	extern void __ptr_glue();
	
	static asm void _ppc_system_call()
	{
		nofralloc
		
		lwz		r12,global_dispatcher	// load dispatcher T-vector
		
		b		__ptr_glue				// cross-TOC call
	}
	
	asm int syscall( int number, ... )
	{
		nofralloc
		
		// copy system call number to r11
		mr  r11,r3
		
		// bump up to six arguments into the correct registers
		mr  r3,r4
		mr  r4,r5
		mr  r5,r6
		mr  r6,r7
		mr  r7,r8
		mr  r8,r9
		
		b   _ppc_system_call
	}
	
	#define DEFINE_SYSCALL( name )\
		extern void name();       \
		asm void name()           \
		{                         \
			li r11,__NR_##name ;  \
			b _ppc_system_call ;  \
		}
	
	#define DEFINE_SYSCALL_7F( name )  DEFINE_SYSCALL( name )
	
	
	// syscalls
	#include "define_syscalls.h"
	
#endif

