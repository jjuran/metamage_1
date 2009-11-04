/*
	alloca-longjmp.cc
	-----------------
	
	Copyright 2009, Joshua Juran
*/

// Standard C
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

// POSIX
#include <unistd.h>

// iota
#include "iota/strings.hh"

// recall
#include "recall/stack_frame.hh"

// tap-out
#include "tap/check.hh"
#include "tap/test.hh"


/*
	
Theory:
	
	The alloca() function is not part of Standard C or POSIX, and consequently
	neither standard has anything to say on its interaction with setjmp/longjmp
	in conforming implementations.  In other words, the effects on a program of
	calling alloca() at all are undefined, and crashing the entire system is a
	perfectly valid (if not quite defensible) result.
	
Practice:
	
	At issue here is this specific interaction between alloca() and longjmp():
	
	* setjmp() is called
	* alloca() is called immediately thereafter
	* the memory returned by alloca() is overwritten
	* longjmp() is called (with the same jmp_buf) before leaving scope
	
	For platforms like 68K where a separate frame pointer (A6) is used than the
	stack pointer (A7), the frame pointer is the backlink, the function-local
	stack area lies above the topmost system data (the previous frame, which
	contains the previous frame pointer and the return address), and the user is
	free to extend the stack without disrupting A6-relative accesses.  The
	implementation of alloca() requires no assistance from the compiler, and the
	scenario above works as expected:  Calling longjmp() undoes the allocation.
	
	By contrast, PowerPC systems use a single stack pointer (r1), requiring a
	system-defined structure at the top of the stack at all times.  While an
	assembler routine may get away with violating this constraint, a compiler-
	generated function cannot -- it must at the very least leave the backlink
	intact, such that it continues to point to the previous stack frame.  Other
	fields in the function's own stack frame are used only by those functions it
	calls, and hence need not be maintained here.
	
	MWCPPC's built-in __alloca() extends the stack by exactly the specified size
	(excepted rounded up to the nearest multiple of eight), writes a copy of the
	stack frame at the new location, and returns a pointer to the memory below
	the new stack frame, which includes its previous location on the stack (at
	the end of the stack-allocated block).  Calling MSL's longjmp() here (having
	called setjmp() before alloca()) would restore the previous stack pointer
	with no ill effects.  But overwriting the overlapping memory first would
	result in a corrupted backlink, which in the absence of means of correction
	would be reloaded into the stack pointer on function exit, followed by an
	attempt to retrieve the return address from the bogus stack frame, which
	either would immediately cause a memory access error, or succeed and result
	in a jump to an arbitrary address.
	
	Obviously, using siglongjmp() instead of longjmp() has the same effect.
	
	Presumably there might be an issue with local variable accesses, since not
	only are we extending the stack (via __alloca(), which the compiler is aware
	of) but also undoing that extension via longjmp() (of which it isn't).  This
	issue is avoided since the compiler uses r31 as a copy of the original stack
	pointer in this case.
	
Rationale:
	
	Since alloca() is not standardized, providing failing tests for its behavior
	is not my prerogative.  Consequently, this test module runs no tests at all
	on platforms other than Lamp.  (For the record, the second and third of each
	group of three tests fail when compiled as either PowerPC or Intel code and
	run on an Intel-based Mac (with Rosetta PowerPC emulation) running Mac OS X
	10.4 "Tiger".)
	
	Lamp currently implements vfork() in terms of sigsetjmp(), and allocates a
	sigjmp_buf on the stack using alloca().  Clearly, alloca() must be called
	first, so that the sigjmp_buf structure is available to pass to sigsetjmp().
	But the pattern above occurs if vfork() is called twice, nested, in the same
	function.  In this case, the first vfork's sigsetjmp() is followed by the
	second vfork's alloca() and eventually the first's siglongjmp(), and the
	described corruption occurs, causing an immediate and fatal system crash.
	
	The most straightforward approach was to modify sigsetjmp() and siglongjmp()
	to save and restore the backlink (i.e. the previous stack pointer saved in
	the current stack frame) in addition to the stack pointer register.
	
	It would be possible (though less convenient) to similarly modify setjmp()
	and longjmp().  This is under consideration, but not a priority, as no other
	functionality depends on it.  Therefore, the final, stack-corrupting test
	remains TODO on PowerPC.
*/

#ifndef __LAMP__
  #define N_TESTS 0
#else
  #define N_TESTS (5 + 3)
#endif
  
#ifdef __POWERPC__
  #define TODO_ALLOCA_LONGJMP " # TODO: alloca/longjmp"
#else
  #define TODO_ALLOCA_LONGJMP ""
#endif

static const unsigned n_tests = N_TESTS;


using tap::ok_if;


static void emergency_exit()
{
	write( STDOUT_FILENO, STR_LEN( "# Fatal stack corruption, exiting\n" ) );
	
	exit( 0 );
}

static void test_alloca_siglongjmp()
{
	const recall::stack_frame* sp0    = recall::get_frame_pointer();
	const recall::stack_frame* sp0_p0 = sp0->next;
	
	volatile int local_variable = 101;
	
	recall::stack_frame const *volatile sp1_p1 = NULL;
	
	sigjmp_buf jump_buffer;
	
	const bool jumped = sigsetjmp( jump_buffer, 1 );
	
	if ( !jumped )
	{
		const size_t size = sizeof (jmp_buf);
		
		void* a = alloca( size );
		
		ok_if( local_variable == 101 );
		
		local_variable = 102;
		
		memset( a, '\0', size );
		
		const recall::stack_frame* sp1 = recall::get_frame_pointer();
		
		sp1_p1 = sp1->next;
		
		siglongjmp( jump_buffer, 1 );
	}
	
	ok_if( local_variable == 102 );
	
	const recall::stack_frame* sp0_p1 = sp0->next;
	
	ok_if( sp0 == recall::get_frame_pointer(), "Original frame pointer intact" );
	
	ok_if( sp1_p1 == sp0_p0, "Linkback unchanged after alloca()" );
	
	ok_if( sp0_p1 == sp0_p0, "Original linkback intact" );
	
	if ( sp0_p1 != sp0_p0 )
	{
		// Stack is corrupted
		emergency_exit();
	}
}

static void test_alloca_longjmp()
{
	const recall::stack_frame* sp0    = recall::get_frame_pointer();
	const recall::stack_frame* sp0_p0 = sp0->next;
	
	recall::stack_frame const *volatile sp1_p1 = NULL;
	
	jmp_buf jump_buffer;
	
	const bool jumped = setjmp( jump_buffer );
	
	if ( !jumped )
	{
		const size_t size = sizeof (jmp_buf);
		
		void* a = alloca( size );
		
		memset( a, '\0', size );
		
		const recall::stack_frame* sp1 = recall::get_frame_pointer();
		
		sp1_p1 = sp1->next;
		
		longjmp( jump_buffer, 1 );
	}
	
	const recall::stack_frame* sp0_p1 = sp0->next;
	
	ok_if( sp0 == recall::get_frame_pointer(), "Original frame pointer intact" );
	
	ok_if( sp1_p1 == sp0_p0, "Linkback unchanged after alloca()" );
	
	ok_if( sp0_p1 == sp0_p0, "Original linkback intact" TODO_ALLOCA_LONGJMP );
	
	if ( sp0_p1 != sp0_p0 )
	{
		// Stack is corrupted
		emergency_exit();
	}
}


int main( int argc, const char *const *argv )
{
	tap::start( "alloca-longjmp", n_tests );
	
	if ( n_tests )
	{
		test_alloca_siglongjmp();
		
		test_alloca_longjmp();
	}
	
	return 0;
}

