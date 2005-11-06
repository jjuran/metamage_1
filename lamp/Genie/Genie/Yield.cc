/*	========
 *	Yield.cc
 *	========
 */

#include "Genie/Yield.hh"

// Nitrogen
#include "Nitrogen/Threads.h"

// POSeven
#include "POSeven/Errno.h"

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
	
	void Yield()
	{
		Process* me = gCurrentProcess;
		
		gCurrentProcess = NULL;
		
		N::YieldToAnyThread();
		
		gCurrentProcess = me;
		
		// Yield() should only be called from the yielding process' thread.
		HandlePendingSignals();
	}
	
	void StopThread( N::ThreadID thread )
	{
		Process* me = gCurrentProcess;
		
		gCurrentProcess = NULL;
		
		N::SetThreadState( thread, kStoppedThreadState );
		
		gCurrentProcess = me;
		
		// StopThread() will only cause a switch if a process stops itself.
		if ( N::GetCurrentThread() == thread )
		{
			// This thread is the one that stopped (and just woke).
			HandlePendingSignals();
		}
	}
	
}

