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
		
		mflr	r0						// get caller's return address
		stw		r0,8(SP)				// store it in caller's stack frame
		
		stwu	SP,-64(SP)				// allocate our own stack frame
		
		lwz		r12,global_dispatcher	// load dispatcher T-vector
		
		bl		__ptr_glue				// cross-TOC call
		nop								// synchronize pipeline
		lwz		RTOC,20(SP)				// restore our RTOC
		
		addi	SP,SP,64				// deallocate our stack frame
		
		lwz		r0,8(SP)				// reload caller's return address
		mtlr	r0						// load it into the link register
		blr								// return
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
	
	#define DEFINE_STUB( name )   \
		extern void name();       \
		asm void name()           \
		{                         \
			li r11,__NR_##name ;  \
			b _ppc_system_call ;  \
		}
	
	#define DEFINE_STUB_7F( name )  DEFINE_STUB( name )
	#define DEFINE_STUB_FF( name )  DEFINE_STUB( name )
	
	
	// syscalls
	#include "define_syscalls.h"
	
#endif

