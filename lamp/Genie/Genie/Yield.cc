/*	========
 *	Yield.cc
 *	========
 */

#include "Genie/Yield.hh"

// Nitrogen
#include "Nitrogen/Threads.h"

// Genie
#include "Genie/Process.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
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
	
	void Yield()
	{
		Process* me = gCurrentProcess;
		
		gCurrentProcess = NULL;
		
		N::YieldToAnyThread();
		
		gCurrentProcess = me;
	}
	
	void StopThread( N::ThreadID thread )
	{
		Process* me = gCurrentProcess;
		
		gCurrentProcess = NULL;
		
		N::SetThreadState( thread, kStoppedThreadState );
		
		gCurrentProcess = me;
	}
	
}

