/*	=========
 *	Faults.cc
 *	=========
 */

#include "Genie/Faults.hh"

// Mac OS
#include <MachineExceptions.h>

// Standard C
#include <signal.h>

// Silver
#include "Silver/Patch.hh"
#include "Silver/Procs.hh"
#include "Silver/Traps.hh"


namespace Genie
{
	
	namespace Ag = Silver;
	
	
	static void BusError()
	{
		DeliverFatalSignal( SIGBUS );
	}
	
	static void IllegalInstruction()
	{
		DeliverFatalSignal( SIGILL );
	}
	
	static void DivisionByZero()
	{
		DeliverFatalSignal( SIGFPE );
	}
	
	static void PrivilegeViolation()
	{
		DeliverFatalSignal( SIGILL );
	}
	
#if TARGET_CPU_68K
	
	extern class Process* gCurrentProcess;  // defined in Process.cc
	
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
	
	static asm void GenericExceptionHandler()
	{
		MOVE.W	6(SP),D0  // get the vector offset
		ANDI.W	#0x0FFF,D0  // mask off frame code
		
		TST.L	gCurrentProcess
		BNE		recover
		
		LEA		gExceptionVectorTable,A0
		MOVEA.L	(A0,D0.W),A0  // get the handler address
		
		JMP		(A0)
		
	recover:
		MOVEA.L	2(SP),A1  // save the stacked PC for later
		
		LEA		generic_68k_recovery_handler,A0
		MOVE.L	A0,2(SP)  // set the stacked PC to the recovery handler
		
		RTE
	}
	
	static void* gExceptionUserHandlerTable[] =
	{
		NULL,  // 0, ISP on reset
		NULL,  // 1, PC on reset
		BusError,
		NULL,  // 3, address error
		IllegalInstruction,
		DivisionByZero
	};
	
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
				
				system_vectors[ i ] = &GenericExceptionHandler;
			}
		}
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
	}
	
	
	struct ExitToShell_Patch
	{
		static void Code( Ag::ExitToShellProcPtr nextHandler )
		{
			remove_68k_exception_handlers();
			
			nextHandler();
		}
	};
	
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
	
	
	void InstallExceptionHandlers()
	{
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
		
		const TVector* handler = NULL;
		
		switch ( exception->theKind )
		{
			case kIllegalInstructionException:
				handler = (const TVector*) IllegalInstruction;
				
				break;
			
			case kAccessException:
			case kUnmappedMemoryException:
			case kUnresolvablePageFaultException:
				handler = (const TVector*) BusError;
				
				break;
			
			case kPrivilegeViolationException:
				handler = (const TVector*) PrivilegeViolation;
				
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
	
}

