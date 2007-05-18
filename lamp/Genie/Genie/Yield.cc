/*	========
 *	Yield.cc
 *	========
 */

#include "Genie/Yield.hh"

// Nitrogen
#include "Nitrogen/Threads.h"

// POSeven
#include "POSeven/Errno.hh"

// Genie
#include "Genie/Process.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace P7 = POSeven;
	
	static Process* gCurrentProcess;
	
	Process& CurrentProcess()
	{
		if ( gCurrentProcess == NULL )
		{
			throw NoSuchProcess();
		}
		
		return *gCurrentProcess;
	}
	
	Process* CurrentProcessContext()
	{
		return gCurrentProcess;
	}
	
	void RegisterProcessContext( Process* process )
	{
		gCurrentProcess = process;
	}
	
	static void HandlePendingSignals()
	{
		bool signalled = gCurrentProcess->HandlePendingSignals();
		
		if ( signalled )
		{
			P7::ThrowErrno( EINTR );
		}
	}
	
	struct ToolScratchGlobals
	{
		int*    err;
		char**  env;
	};
	
	static ToolScratchGlobals& gToolScratchGlobals = *reinterpret_cast< ToolScratchGlobals* >( LMGetToolScratch() );
	
	static UInt32 gTickCountOfLastSleep = 0;
	
	static const UInt32 gMinimumSleepIntervalTicks = 30;  // Sleep every half second
	
	void Breathe()
	{
		UInt32 now = ::TickCount();
		
		if ( now - gTickCountOfLastSleep > gMinimumSleepIntervalTicks )
		{
			Yield();
			
			gTickCountOfLastSleep = now;
		}
	}
	
	void Yield()
	{
		Process* me = gCurrentProcess;
		
		me->SetSchedule( kProcessSleeping );
		
		gCurrentProcess = NULL;
		
		N::YieldToAnyThread();
		
		gCurrentProcess = me;
		
		gToolScratchGlobals.env = me->Environ();
		
		me->SetSchedule( kProcessRunning );
		
		// Yield() should only be called from the yielding process' thread.
		HandlePendingSignals();
		
		gTickCountOfLastSleep = ::TickCount();
	}
	
	void StopThread( N::ThreadID thread )
	{
		Process* me = gCurrentProcess;
		
		gCurrentProcess = NULL;
		
		N::SetThreadState( thread, N::kStoppedThreadState );
		
		gCurrentProcess = me;
		
		// StopThread() will only cause a switch if a process stops itself.
		if ( N::GetCurrentThread() == thread )
		{
			// This thread is the one that stopped (and just woke).
			me->SetSchedule( kProcessRunning );
			
			HandlePendingSignals();
		}
	}
	
	void ReadyThread( N::ThreadID thread )
	{
		Process* me = gCurrentProcess;
		
		gCurrentProcess = NULL;
		
		N::SetThreadState( thread, N::kReadyThreadState );
		
		gCurrentProcess = me;
		
		if ( N::GetCurrentThread() == thread )
		{
			HandlePendingSignals();
		}
	}
	
}

