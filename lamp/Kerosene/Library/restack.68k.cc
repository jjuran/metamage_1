/*
	restack.68k.cc
	--------------
*/

// Lamp
#include "lamp/restack.h"


extern "C" void* _create_new_stack();

#ifdef __MC68K__

asm void _lamp_restack( unsigned param_length, const void* f, ... )
{
	LINK  A6, #-4
	
	JSR  _create_new_stack
	LEA  -8(A0), A1  // A1 <= &stack.stack_limit
	
	// initialize dummy frame in stack footer
	MOVE.L  4(A6), -(A1)
	MOVE.L   (A6), -(A1)
	
	MOVE.L   8(A6), D1  // param length
	
	SUBA.L  D1, A1  // A1 <= low-memory edge of param area (was high-mem edge)
	
	MOVE.L   A1, -4(A6)  // save new stack pointer
	
	LEA     16(A6),  A0  // address of first parameter
	
	BRA.S   loop_test
loop_begin:
	MOVE.B  (A0)+, (A1)+
loop_test:
	DBRA    D1, loop_begin
	
	MOVEA.L 12(A6), A0   // copy function address to a0
	
	MOVE.L   4(A6), 8(A6)  // move return address to elide f
	
	MOVEA.L  -4(A6), A7  // switch to new stack
	
	JSR      (A0)
	
	UNLK  A6
	
	RTS
}

#endif

