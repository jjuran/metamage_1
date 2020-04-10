/*
	stack_frame.hh
	
	Copyright 2007-2009, Joshua Juran
*/

#ifndef RECALL_STACKFRAME_HH
#define RECALL_STACKFRAME_HH

// recall
#include "recall/return_address.hh"


namespace recall
{
	
	struct stack_frame_68k
	{
		stack_frame_68k*    next;
		return_address_68k  return_address;
	};
	
	struct stack_frame_ppc
	{
		stack_frame_ppc*    next;
		const void*         saved_CR;
		return_address_ppc  return_address;
	};
	
	struct stack_frame_x86
	{
		stack_frame_x86*       next;
		return_address_native  return_address;
	};
	
	
#ifdef __MC68K__
	
	typedef stack_frame_68k stack_frame;
	
	#pragma parameter __D0 get_A6
	
	inline char* get_A6() = { 0x200e };
	
	inline const stack_frame_68k* get_frame_pointer()
	{
		return (const stack_frame_68k*) get_A6();
	}
	
	inline const stack_frame_68k* get_top_frame()
	{
		return get_frame_pointer();
	}
	
#endif

#if defined( __POWERPC__ )  ||  defined( __powerpc__ )
	
	typedef stack_frame_ppc stack_frame;
	
	#ifdef __MWERKS__
		
		static asm char *get_r1( void )
		{
			mr		r3,r1
			blr
		}
		
	#endif
	
	#ifdef __GNUC__
		
		static char *get_r1( void )
		{
			char* result;
			
			__asm__( "mr r3,r1" : "=r"(result) );
			
			return result;
		}
		
	#endif
	
	inline const stack_frame_ppc* get_frame_pointer()
	{
		return (const stack_frame_ppc*) get_r1();
	}
	
	inline const stack_frame_ppc* get_top_frame()
	{
		return get_frame_pointer()->next;
	}
	
#endif
	
#ifdef __i386__
	
	typedef stack_frame_x86 stack_frame;
	
	static char *get_ebp( void )
	{
		char* result;
		
		__asm__( "mov  %%ebp,%0" : "=ra"(result) );
		
		return result;
	}
	
	inline const stack_frame_x86* get_frame_pointer()
	{
		return (const stack_frame_x86*) get_ebp();
	}
	
	inline const stack_frame_x86* get_top_frame()
	{
		return get_frame_pointer()->next;
	}
	
#endif
	
#ifdef __x86_64__
	
	typedef stack_frame_x86 stack_frame;
	
	static char *get_rbp( void )
	{
		char* result;
		
		__asm__( "mov  %%rbp,%0" : "=ra"(result) );
		
		return result;
	}
	
	inline const stack_frame_x86* get_frame_pointer()
	{
		return (const stack_frame_x86*) get_rbp();
	}
	
	inline const stack_frame_x86* get_top_frame()
	{
		return get_frame_pointer()->next;
	}

#endif
	
}

#endif
