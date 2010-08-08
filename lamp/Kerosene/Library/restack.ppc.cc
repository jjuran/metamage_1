/*
	restack.ppc.cc
	--------------
*/

// Lamp
#include "lamp/restack.h"


extern "C" void* _create_new_stack();

#ifdef __POWERPC__

extern "C" void __ptr_glue();

asm void _lamp_restack( unsigned param_length, const void* f, ... )
{
	// automatic prolog
	
	stw  r3,  24(sp)
	stw  r4,  28(sp)
	stw  r5,  32(sp)
	stw  r6,  36(sp)
	stw  r7,  40(sp)
	stw  r8,  44(sp)
	stw  r9,  48(sp)
	stw  r10, 52(sp)
	
	// allocate stack frame
	stwu  sp, -32(sp)
	
	bl  _create_new_stack
	
	addi  r11,r3,(-sizeof (_lamp_stack_footer))  // r11 <= &stack.frame_pointer
	
	// initialize dummy frame in stack footer
	lwz  r5, 0(sp)
	lwz  r7, 8(sp)
	
	stw  r5, 0(r11)
	stw  r7, 8(r11)
	
	lwz  r12, 60(sp)  // function
	
	// load params into registers, ignoring param length
	lwz  r3, 64(sp)
	lwz  r4, 68(sp)
	lwz  r5, 72(sp)
	lwz  r6, 76(sp)
	lwz  r7, 80(sp)
	lwz  r8, 84(sp)
	
	mr  sp, r11  // switch to new stack
	
	bl  __ptr_glue
	nop
	
	lwz  sp, 0(sp)  // pop
	
	lwz   r0, 8(sp)
	mtlr  r0
	
	blr
}

#endif

