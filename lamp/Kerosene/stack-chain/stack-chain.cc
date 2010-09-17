/*
	stack-chain.cc
	--------------
*/

// Standard C
#include <signal.h>
#include <stdlib.h>

// Lamp
#include "lamp/parameter_block.h"
#include "lamp/restack.h"
#include "lamp/stack.h"
#include "tool-runtime/parameter_block.h"


#ifdef __MC68K__
#define GETSP( sp )  MOVE.L  SP, sp
#endif

#ifdef __POWERPC__
#define GETSP( sp )  mr  sp, SP
#endif


extern "C" void* _create_new_stack();


static _lamp_stack_footer* global_stack = NULL;


static _lamp_stack_footer* get_top_stack()
{
	register const void* stack_pointer = 0;
	
	asm
	{
		GETSP( stack_pointer );
	}
	
	while ( global_stack != NULL )
	{
		void* stack_limit = global_stack->stack_limit;
		
		if ( stack_limit < stack_pointer  &&  stack_pointer < global_stack )
		{
			return global_stack;
		}
		
		global_stack = global_stack->previous;
		
		free( stack_limit );
	}
	
	return NULL;
}

static inline const void* get_stack_limit()
{
	if ( _lamp_stack_footer* stack = get_top_stack() )
	{
		return stack->stack_limit;
	}
	
	return global_user_params->stack_limit;
}

unsigned _lamp_stack_space()
{
	register const char* stack_pointer = 0;
	
	asm
	{
		GETSP( stack_pointer );
	}
	
	const char* stack_limit = (const char*) get_stack_limit();
	
	return stack_pointer - stack_limit;
}

void* _create_new_stack()
{
	const size_t size = 64 * 1024;
	
	void* limit = malloc( size );
	
	if ( limit == NULL )
	{
		raise( SIGSTKFLT );
	}
	
	void* base = (char*) limit + size;
	
	_lamp_stack_footer* footer = (_lamp_stack_footer*) base - 1;
	
	footer->stack_limit = limit;
	footer->previous    = global_stack;
	
	global_stack = footer;
	
	return base;
}

