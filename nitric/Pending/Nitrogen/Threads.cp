// Threads.cp

#ifndef NITROGEN_THREADS_H
#include "Nitrogen/Threads.h"
#endif

namespace Nitrogen
{
	
	Owned< ThreadID > NewThread( ThreadStyle     threadStyle,
	                             ThreadEntryTPP  threadEntry,
	                             void*           threadParam,
	                             Size            stackSize,
	                             ThreadOptions   options,
	                             void**          threadResult )
	{
		OnlyOnce< RegisterThreadManagerErrors >();
		
		::ThreadID result;
		
		ThrowOSStatus( ::NewThread( threadStyle,
		                            threadEntry,
		                            threadParam,
		                            stackSize,
		                            options,
		                            threadResult,
		                            &result ) );
		
		return Owned< ThreadID >::Seize( ThreadID( result ) );
	}
	
	void SetThreadScheduler( ThreadSchedulerTPP threadScheduler )
	{
		OnlyOnce< RegisterThreadManagerErrors >();
		
		ThrowOSStatus( ::SetThreadScheduler( threadScheduler ) );
	}
	
	void SetThreadSwitcher( ThreadID         thread,
	                        ThreadSwitchTPP  threadSwitcher,
	                        void*            switchProcParam,
	                        bool             inVsOut )
	{
		OnlyOnce< RegisterThreadManagerErrors >();
		
		ThrowOSStatus( ::SetThreadSwitcher( thread, threadSwitcher, switchProcParam, inVsOut ) );
	}
	
	void SetThreadTerminator( ThreadID              thread,
	                          ThreadTerminationTPP  threadTerminator,
	                          void *                terminationProcParam )
	{
		OnlyOnce< RegisterThreadManagerErrors >();
		
		ThrowOSStatus( ::SetThreadTerminator( thread, threadTerminator, terminationProcParam ) );
	}
	
	void SetDebuggerNotificationProcs( DebuggerNewThreadTPP        notifyNewThread,
	                                   DebuggerDisposeThreadTPP    notifyDisposeThread,
	                                   DebuggerThreadSchedulerTPP  notifyThreadScheduler )
	{
		OnlyOnce< RegisterThreadManagerErrors >();
		
		ThrowOSStatus( ::SetDebuggerNotificationProcs( notifyNewThread,
		                                               notifyDisposeThread,
		                                               notifyThreadScheduler ) );
	}
	
	void CreateThreadPool( ThreadStyle  threadStyle,
	                       std::size_t  numToCreate,
	                       Size         stackSize )
	{
		OnlyOnce< RegisterThreadManagerErrors >();
		
		ThrowOSStatus( ::CreateThreadPool( threadStyle, numToCreate, stackSize ) );
	}
	
	std::size_t GetFreeThreadCount( ThreadStyle threadStyle )
	{
		OnlyOnce< RegisterThreadManagerErrors >();
		
		SInt16 result;
		ThrowOSStatus( ::GetFreeThreadCount( threadStyle, &result ) );
		
		return result;
	}
	
	std::size_t GetSpecificFreeThreadCount( ThreadStyle  threadStyle,
	                                        Size         stackSize )
	{
		OnlyOnce< RegisterThreadManagerErrors >();
		
		SInt16 result;
		ThrowOSStatus( ::GetSpecificFreeThreadCount( threadStyle, stackSize, &result ) );
		
		return result;
	}
	
	Size GetDefaultThreadStackSize( ThreadStyle threadStyle )
	{
		OnlyOnce< RegisterThreadManagerErrors >();
		
		::Size result;
		ThrowOSStatus( ::GetDefaultThreadStackSize( threadStyle, &result ) );
		
		return result;
	}
	
	std::size_t ThreadCurrentStackSpace( ThreadID thread )
	{
		OnlyOnce< RegisterThreadManagerErrors >();
		
		UInt32 result;
		ThrowOSStatus( ::ThreadCurrentStackSpace( thread, &result ) );
		
		return result;
	}
	
	void DisposeThread( Owned< ThreadID >  thread,
	                    void*              threadResult,
	                    bool               recycleThread )
	{
		OnlyOnce< RegisterThreadManagerErrors >();
		
		ThrowOSStatus( ::DisposeThread( thread.Release(), threadResult, recycleThread ) );
	}
	
	void YieldToThread( ThreadID suggestedThread )
	{
		OnlyOnce< RegisterThreadManagerErrors >();
		
		ThrowOSStatus( ::YieldToThread( suggestedThread ) );
	}
	
	void YieldToAnyThread()
	{
		OnlyOnce< RegisterThreadManagerErrors >();
		
		ThrowOSStatus( ::YieldToAnyThread() );
	}
	
	ThreadID MacGetCurrentThread()
	{
		OnlyOnce< RegisterThreadManagerErrors >();
		
		::ThreadID result;
		ThrowOSStatus( ::MacGetCurrentThread( &result ) );
		
		return result;
	}
	
	ThreadState GetThreadState( ThreadID thread )
	{
		OnlyOnce< RegisterThreadManagerErrors >();
		
		::ThreadState result;
		ThrowOSStatus( ::GetThreadState( thread, &result ) );
		
		return result;
	}
	
	void SetThreadState( ThreadID     threadToSet,
	                     ThreadState  newState,
	                     ThreadID     suggestedThread )
	{
		OnlyOnce< RegisterThreadManagerErrors >();
		
		ThrowOSStatus( ::SetThreadState( threadToSet,
		                                 newState,
		                                 suggestedThread ) );
	}
	
	void SetThreadStateEndCritical( ThreadID     threadToSet,
	                                ThreadState  newState,
	                                ThreadID     suggestedThread )
	{
		OnlyOnce< RegisterThreadManagerErrors >();
		
		ThrowOSStatus( ::SetThreadStateEndCritical( threadToSet,
		                                            newState,
		                                            suggestedThread ) );
	}
	
	void ThreadBeginCritical()
	{
		OnlyOnce< RegisterThreadManagerErrors >();
		
		ThrowOSStatus( ::ThreadBeginCritical() );
	}
	
	void ThreadEndCritical()
	{
		OnlyOnce< RegisterThreadManagerErrors >();
		
		ThrowOSStatus( ::ThreadEndCritical() );
	}
	
	ThreadTaskRef GetThreadCurrentTaskRef()
	{
		OnlyOnce< RegisterThreadManagerErrors >();
		
		::ThreadTaskRef result;
		ThrowOSStatus( ::GetThreadCurrentTaskRef( &result ) );
		
		return result;
	}
	
	ThreadState GetThreadStateGivenTaskRef( ThreadTaskRef  threadTaskRef,
	                                        ThreadID       threadToGet )
	{
		OnlyOnce< RegisterThreadManagerErrors >();
		
		::ThreadState result;
		ThrowOSStatus( ::GetThreadStateGivenTaskRef( threadTaskRef,
		                                             threadToGet,
		                                             &result ) );
		
		return result;
	}
	
	void SetThreadReadyGivenTaskRef( ThreadTaskRef  threadTaskRef,
	                                 ThreadID       threadToSet )
	{
		OnlyOnce< RegisterThreadManagerErrors >();
		
		ThrowOSStatus( ::SetThreadReadyGivenTaskRef( threadTaskRef, threadToSet ) );
	}
	
	void RegisterThreadManagerErrors()
	{
		RegisterOSStatus< paramErr             >();
		RegisterOSStatus< memFullErr           >();
		RegisterOSStatus< threadTooManyReqsErr >();
		RegisterOSStatus< threadNotFoundErr    >();
		RegisterOSStatus< threadProtocolErr    >();
	}
	
}

