/*
	stack-space.cc
	--------------
*/

// Relix
#include "relix/parameter_block.h"
#include "relix/stack.h"
#include "tool-runtime/parameter_block.h"


#ifdef __MC68K__
#define GETSP( sp )  MOVE.L  SP, sp
#endif

#ifdef __POWERPC__
#define GETSP( sp )  mr  sp, SP
#endif


unsigned _relix_stack_space()
{
	register const char* stack_pointer = 0;
	
	asm
	{
		GETSP( stack_pointer );
	}
	
	const char* stack_limit = (const char*) global_user_params->stack_limit;
	
	return stack_pointer - stack_limit;
}

