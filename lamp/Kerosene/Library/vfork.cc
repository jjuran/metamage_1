/*	========
 *	vfork.cc
 *	========
 */

// POSIX
#include <unistd.h>

// Relix
#include "relix/vfork.h"


#pragma exceptions off


// System call
extern "C" pid_t vfork_start( const _vfork_pad* pad );


static void _resume_vfork( const _vfork_pad* pad );


static pid_t _vfork( _vfork_pad* pad )
{
	pad->resume_handler = &_resume_vfork;
	
	return vfork_start( pad );
}

#ifdef __MC68K__

asm pid_t vfork()
{
	MOVEA.L  (A7),           A1     // save return address in A1
	
	LINK     A6, #0
	
	PEA      8(A6)                  // push the pre-vfork SP
	MOVE.L   (A6),           -(A7)  // push the pre-LINK A6
	MOVEM.L  D0/D3-D7/A1-A4, -(A7)  // save remaining registers
	
	PEA      (A7)                   // push our stack-based pad onto the stack
	
	JSR		 _vfork
	
	UNLK     A6
	
	RTS
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
	
	mflr  r0
	stw   r0,8(SP)
	
	mr  r6,SP
	
	stwu  SP,-272(SP)
	
	addi  r3,SP,24
	
	mfcr  r5
	
	stw  r0,4(r3)
	stw  r5,8(r3)
	
	lwz  r4,0(r6)
	
	stw  r6,12(r3)
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
	
	bl  _vfork
	
	addi  SP,SP,272
	
	lwz   r0,8(SP)
	mtlr  r0
	blr
}

static asm void _resume_vfork( const _vfork_pad* pad )
{
	nofralloc
	
	lwz  r4,4(r3)
	lwz  r5,8(r3)
	
	mtlr  r4
	mtcr  r5
	
	lwz  r4,16(r3)
	lwz  r6,12(r3)
	
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
	
	stw  r4,0(r6)
	
	lwz  r3,0(r3)
	
	mr  r1,r6
	
	blr
}

#endif
