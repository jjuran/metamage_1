/*	=========
 *	Faults.cc
 *	=========
 */

#include "Genie/Faults.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __MACHINEEXCEPTIONS__
#include <MachineExceptions.h>
#endif

// Silver
#if TARGET_CPU_68K
#include "Silver/Patch.hh"
#include "Silver/Procs.hh"
#include "Silver/Traps.hh"
#endif

// relix
#include "relix/glue/exceptions.hh"
#include "relix/glue/system_call.68k.hh"


namespace relix
{
	
	extern class Process* gCurrentProcess;  // defined in Process.cc
	
	
#if TARGET_CPU_68K
	
	namespace Ag = Silver;
	
	
	extern void* gExceptionVectorTable[];
	extern void* gExceptionUserHandlerTable[];
	
	static asm void generic_68k_recovery_handler()
	{
		// D0 is the vector offset
		// A1 is the PC reported in the exception stack frame
		
		MOVE.L	A1,-(SP)  // push PC as if it were a return address
		
		LEA		gExceptionUserHandlerTable,A0
		MOVEA.L	(A0,D0.W),A0  // get the handler address
		
		JMP		(A0)
	}
	
	#define DEFINE_EXCEPTION_HANDLER(N) \
	static asm void generic_exception_handler_ ## N() \
	{ \
		TST.L   gCurrentProcess                  ;\
		BNE     recover                          ;\
		\
		/* get the handler address */ \
		LEA     (gExceptionVectorTable + N),A0   ;\
		MOVEA.L (A0),A0                          ;\
		\
		JMP     (A0)                             ;\
		\
	recover: \
		/* save the stacked PC for later */ \
		MOVEA.L 2(SP),A1                         ;\
		MOVEQ.L	#(N * sizeof (void*)),D0         ;\
		\
		/* set the stacked PC to the recovery handler */ \
		LEA     generic_68k_recovery_handler,A0  ;\
		MOVE.L  A0,2(SP)                         ;\
		\
		RTE \
	} extern int dummy_for_semicolon
	
	DEFINE_EXCEPTION_HANDLER(2);
	DEFINE_EXCEPTION_HANDLER(3);
	DEFINE_EXCEPTION_HANDLER(4);
	DEFINE_EXCEPTION_HANDLER(5);
	DEFINE_EXCEPTION_HANDLER(6);
	DEFINE_EXCEPTION_HANDLER(7);
	DEFINE_EXCEPTION_HANDLER(8);
	
	#undef DEFINE_EXCEPTION_HANDLER
	
	static void* gExceptionUserHandlerTable[] =
	{
		NULL,  // 0, ISP on reset
		NULL,  // 1, PC on reset
		bus_error,
		address_error,
		illegal_instruction,
		division_by_zero,
		integer_range_check,
		integer_overflow,
		privilege_violation,
	};
	
	
	static void* saved_trap_2_handler = NULL;
	
	static asm void trap_2_exception_handler()
	{
		TST.L	gCurrentProcess
		BNE		recover
		
		MOVEA.L	saved_trap_2_handler,A0  // get the handler address
		
		JMP		(A0)
		
	recover:
		LEA		dispatch_68k_system_call,A0
		MOVE.L	A0,2(SP)  // set the stacked PC to the dispatcher
		
		RTE
	}
	
	static const unsigned n_vectors = sizeof gExceptionUserHandlerTable / sizeof (void*);
	
	static void* gExceptionVectorTable[ n_vectors ];
	
	static void install_68k_exception_handlers()
	{
		void** const system_vectors = 0L;
		
		for ( unsigned i = 0;  i < n_vectors;  ++i  )
		{
			if ( gExceptionUserHandlerTable[ i ] != NULL )
			{
				gExceptionVectorTable[ i ] = system_vectors[ i ];
			}
		}
		
		#define INSTALL_EXCEPTION_HANDLER(n) \
		system_vectors[ n ] = &generic_exception_handler_##n
		
		INSTALL_EXCEPTION_HANDLER( 2 );  // bus error
		INSTALL_EXCEPTION_HANDLER( 3 );  // address error
		INSTALL_EXCEPTION_HANDLER( 4 );  // illegal instruction
		INSTALL_EXCEPTION_HANDLER( 5 );  // division by zero
		INSTALL_EXCEPTION_HANDLER( 6 );  // CHK
		INSTALL_EXCEPTION_HANDLER( 7 );  // TRAPV
		INSTALL_EXCEPTION_HANDLER( 8 );  // privilege violation
		
		#undef INSTALL_EXCEPTION_HANDLER
		
		saved_trap_2_handler = system_vectors[ 32 + 2 ];
		
		system_vectors[ 32 + 2 ] = trap_2_exception_handler;
	}
	
	static void remove_68k_exception_handlers()
	{
		void** const system_vectors = 0L;
		
		for ( unsigned i = 0;  i < n_vectors;  ++i  )
		{
			if ( gExceptionUserHandlerTable[ i ] != NULL )
			{
				system_vectors[ i ] = gExceptionVectorTable[ i ];
			}
		}
		
		system_vectors[ 32 + 2 ] = saved_trap_2_handler;
	}
	
	
	struct GetNextEvent_Patch
	{
		static short Code( EventMask eventMask, EventRecord* theEvent, Ag::GetNextEventProcPtr nextHandler );
	};
	
	short GetNextEvent_Patch::Code( EventMask eventMask, EventRecord* theEvent, Ag::GetNextEventProcPtr nextHandler )
	{
		remove_68k_exception_handlers();
		
		short result = nextHandler( eventMask, theEvent );
		
		install_68k_exception_handlers();
		
		return result;
	}
	
	struct ExitToShell_Patch
	{
		static void Code( Ag::ExitToShellProcPtr nextHandler )
		{
			Ag::TrapPatch< _GetNextEvent, GetNextEvent_Patch::Code >::Remove();
			Ag::TrapPatch< _ExitToShell,  ExitToShell_Patch::Code  >::Remove();
			
			remove_68k_exception_handlers();
			
			nextHandler();
		}
	};
	
	
	void InstallExceptionHandlers()
	{
		install_68k_exception_handlers();
		
		Ag::TrapPatch< _ExitToShell,  ExitToShell_Patch::Code  >::Install();
		Ag::TrapPatch< _GetNextEvent, GetNextEvent_Patch::Code >::Install();
	}
	
#endif
	
#if TARGET_CPU_PPC
	
	typedef unsigned long word_32;
	
	struct TVector
	{
		word_32 f;
		word_32 toc;
	};
	
	static inline void push( word_32& address, int offset )
	{
		word_32 updated_address = address + offset;
		
		*(word_32*) updated_address = address;
		
		address = updated_address;
	}
	
	static OSStatus ExceptionHandler( ExceptionInformation* exception )
	{
		if ( exception->theKind == kTraceException )
		{
			return -1;  // handled by debugger
		}
		
		if ( gCurrentProcess == NULL )
		{
			return -1;
		}
		
		const TVector* handler = NULL;
		
		switch ( exception->theKind )
		{
			case kIllegalInstructionException:
				handler = (const TVector*) illegal_instruction;
				
				break;
			
			case kAccessException:
			case kUnmappedMemoryException:
			case kUnresolvablePageFaultException:
				handler = (const TVector*) bus_error;
				
				break;
			
			case kPrivilegeViolationException:
				handler = (const TVector*) privilege_violation;
				
				break;
			
			default:
				return -1;
		}
		
		// caller: main() in our test cases
		// offender: function that messed up
		// exception handler: exits after tweaking things so we resume at...
		// recovery handler: delivers a fatal signal and doesn't return
		
		// Crawl the stack one step.
		
		// The last stack frame allocated
		word_32& stack_pointer = exception->registerImage->R1.lo;
		
		// The LR when an exception was taken (in the offender), points to caller
		const word_32 link_register = exception->machineState->LR.lo;
		
		// The previous stack frame
		const word_32 prev_stack_pointer = *(word_32*) stack_pointer;
		
		// The most recent saved LR
		const word_32 prev_saved_link = *(word_32*) ( prev_stack_pointer + 8 );
		
		// Did the offender build a stack frame?
		// The offender's LR (link_register) points to the caller.
		// If the offender built a stack frame, then that's the most recent,
		// and the previous one was built by the caller.
		// If that's the case, then the offender would have saved the LR in the
		// caller's stack frame.
		// Otherwise, the topmost frame belongs to the caller, and the previous
		// frame to the caller's caller, and the LR won't match.
		const bool offender_has_stack_frame = prev_saved_link == link_register;
		
		if ( !offender_has_stack_frame )
		{
			// Offender doesn't have a stack frame; let's build one
			
			// This is the saved LR slot in the current stack frame
			word_32& saved_link = *(word_32*) ( stack_pointer + 8 );
			
			// store caller's return address in caller's stack frame
			saved_link = link_register;
			
			// push a new stack frame; hope we aren't stomping anything
			push( stack_pointer, -32 );
		}
		
		// set offender's PC as return address for recovery handler
		exception->machineState ->LR.lo = exception->machineState->PC.lo;
		
		// set new PC and TOC
		exception->machineState ->PC.lo = handler->f;
		exception->registerImage->R2.lo = handler->toc;
		
		return noErr;
	}
	
	void InstallExceptionHandlers()
	{
		static ExceptionHandlerUPP upp = ::NewExceptionHandlerUPP( ExceptionHandler );
		
		::InstallExceptionHandler( upp );
	}
	
#endif
	
#if TARGET_CPU_X86
	
	void InstallExceptionHandlers()
	{
		// do nothing
	}
	
#endif
	
}
