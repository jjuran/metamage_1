/*	=========
 *	Faults.cc
 *	=========
 */

#include "Genie/Faults.hh"

// Mac OS
#include <MachineExceptions.h>

// Standard C
#include <signal.h>

// Genie
#include "Genie/Process.hh"


namespace Genie
{
	
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
	
	extern Process* gCurrentProcess;  // defined in Process.cc
	
	extern void* gExceptionVectorTable[];
	extern void* gExceptionUserHandlerTable[];
	
	static asm void GenericExceptionHandler()
	{
		MOVE.W	6(SP),D0  // get the vector offset
		ANDI.W	#0x0FFF,D0  // mask off frame code
		
		TST.L	gCurrentProcess
		BEQ		null
		
		MOVE	USP,A1
		MOVE.L	2(SP),-(A1)  // push old PC as return address for stack crawls
		MOVE	A1,USP
		
		LEA		gExceptionUserHandlerTable,A0
		ADDA.W	D0,A0  // add the offset
		
		MOVE.L	(A0),2(SP)  // set stacked PC to the handler address
		
		BRA		end
		
	null:
		LEA		gExceptionVectorTable,A0
		ADDA.W	D0,A0  // add the offset
		MOVEA.L	(A0),A0  // get the handler address
		
		JMP		(A0)
		
	end:
		RTE
	}
	
	static void* gExceptionVectorTable[] =
	{
		NULL,  // 0, ISP on reset
		NULL,  // 1, PC on reset
		GenericExceptionHandler,
		NULL,  // 3, address error
		GenericExceptionHandler,
		GenericExceptionHandler
	};
	
	static void* gExceptionUserHandlerTable[] =
	{
		NULL,  // 0, ISP on reset
		NULL,  // 1, PC on reset
		BusError,
		NULL,  // 3, address error
		IllegalInstruction,
		DivisionByZero
	};
	
	void InstallExceptionHandlers()
	{
		/*
		const std::size_t n_vectors = sizeof gExceptionVectorTable / sizeof gExceptionVectorTable[0];
		
		for ( unsigned i = 0;  i < n_vectors;  ++i  )
		{
			if ( gExceptionVectorTable[ i ] != NULL )
			{
				void **const v = (void**) ( i * sizeof (void*) );
				
				std::swap( *v, gExceptionVectorTable[ i ] );
			}
		}
		*/
	}
	
#endif
	
#if TARGET_CPU_PPC
	
	struct TVector
	{
		unsigned long f;
		unsigned long toc;
	};
	
	inline void push( unsigned long& address, int offset )
	{
		unsigned long updated_address = address + offset;
		
		*(unsigned long*) updated_address = address;
		
		address = updated_address;
	}
	
	static OSStatus GenericExceptionHandler( ExceptionInformation* exception )
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
		unsigned long previousSP = *(unsigned long*) exception->registerImage->R1.lo;
		
		unsigned long previousSavedLR = *(unsigned long*) ( previousSP + 8 );
		
		bool offenderHasStackFrame = previousSavedLR == exception->machineState->LR.lo;
		
		if ( !offenderHasStackFrame )
		{
			// Offender doesn't have a stack frame; let's build one
			
			unsigned long& savedLR = *(unsigned long*) ( exception->registerImage->R1.lo + 8 );
			
			// store caller's return address in caller's stack frame
			savedLR = exception->machineState->LR.lo;
			
			// push a new stack frame; hope we aren't stomping anything
			push( exception->registerImage->R1.lo, -32 );
		}
		
		// place offender's return address in LR for recovery handler
		exception->machineState ->LR.lo = exception->machineState->PC.lo;
		
		// set new PC and TOC
		exception->machineState ->PC.lo = handler->f;
		exception->registerImage->R2.lo = handler->toc;
		
		return noErr;
	}
	
	void InstallExceptionHandlers()
	{
		static ExceptionHandlerUPP upp = ::NewExceptionHandlerUPP( GenericExceptionHandler );
		
		::InstallExceptionHandler( upp );
	}
	
#endif
	
}

