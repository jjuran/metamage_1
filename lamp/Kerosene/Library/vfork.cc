/*	========
 *	vfork.cc
 *	========
 */

// POSIX
#include <unistd.h>

// Lamp
#include "lamp/environ_stack.h"
#include "lamp/vfork.h"


#pragma exceptions off


// System call
extern "C" pid_t vfork_start( _resume_handler_t handler, const _vfork_pad* pad );


static void _resume_vfork( const _vfork_pad* pad );


static _vfork_pad global_pad;


static void resume_vfork( const _vfork_pad* pad )
{
	_pop_environ();
	
	_resume_vfork( pad );
}


static pid_t _vfork()
{
	pid_t result = vfork_start( &resume_vfork, &global_pad );
	
	if ( result == 0 )
	{
		_push_environ();
	}
	
	return result;
}

#ifdef __MC68K__

asm pid_t vfork()
{
	MOVEA.L  (A7),              A1             // save return address in A1
	
	MOVEM.L  D3-D7/A1-A4/A6/A7, global_pad.d3  // save remaining registers
	ADDQ.L   #4,                global_pad.sp  // pop the saved stack
	
	JMP		 _vfork
}

static asm void _resume_vfork( const _vfork_pad* pad )
{
	MOVEA.L  4(A7), A0                    // load pad address to A0
	MOVEM.L  (A0),  D0/D3-D7/A1-A4/A6/A7  // restore registers
	JMP      (A1)                         // jump to saved return address
}

#endif


#ifdef __POWERPC__

asm pid_t vfork()
{
	nofralloc
	
	lwz   r3,global_pad
	
	mflr  r0
	mfcr  r5
	
	stw  r0,4(r3)
	stw  r5,8(r3)
	
	lwz  r4,0(r1)
	
	stw  r1,12(r3)
	stw  r4,16(r3)
	
	mffs fp0
	
	stmw  r13,20(r3)
	
	stfd  fp0,96(r3)
	
	#define STORE_FLOAT( i )  stfd  fp##i,(104 + 8 * (i - 14))(r3)
	
	STORE_FLOAT( 14 )
	STORE_FLOAT( 15 )
	STORE_FLOAT( 16 )
	STORE_FLOAT( 17 )
	STORE_FLOAT( 18 )
	STORE_FLOAT( 19 )
	STORE_FLOAT( 20 )
	STORE_FLOAT( 21 )
	STORE_FLOAT( 22 )
	STORE_FLOAT( 23 )
	STORE_FLOAT( 24 )
	STORE_FLOAT( 25 )
	STORE_FLOAT( 26 )
	STORE_FLOAT( 27 )
	STORE_FLOAT( 28 )
	STORE_FLOAT( 29 )
	STORE_FLOAT( 30 )
	STORE_FLOAT( 31 )
	
	b  _vfork
}

static asm void _resume_vfork( const _vfork_pad* pad )
{
	nofralloc
	
	lwz  r4,4(r3)
	lwz  r5,8(r3)
	
	mtlr  r4
	mtcr  r5
	
	lwz  r4,16(r3)
	lwz  r1,12(r3)
	
	stw  r4,0(r1)
	
	lfd  fp0,96(r3)
	
	lmw  r13,20(r3)
	
	#define LOAD_FLOAT( i )  lfd  fp##i,(104 + 8 * (i - 14))(r3)
	
	LOAD_FLOAT( 14 )
	LOAD_FLOAT( 15 )
	LOAD_FLOAT( 16 )
	LOAD_FLOAT( 17 )
	LOAD_FLOAT( 18 )
	LOAD_FLOAT( 19 )
	LOAD_FLOAT( 20 )
	LOAD_FLOAT( 21 )
	LOAD_FLOAT( 22 )
	LOAD_FLOAT( 23 )
	LOAD_FLOAT( 24 )
	LOAD_FLOAT( 25 )
	LOAD_FLOAT( 26 )
	LOAD_FLOAT( 27 )
	LOAD_FLOAT( 28 )
	LOAD_FLOAT( 29 )
	LOAD_FLOAT( 30 )
	LOAD_FLOAT( 31 )
	
	mtfsf  0xff,fp0
	
	lwz  r3,0(r3)
	
	blr
}

#endif

