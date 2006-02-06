// Threads.cp

#ifndef NITROGEN_THREADS_H
#include "Nitrogen/Threads.h"
#endif

namespace Nitrogen
{
	
	Nucleus::Owned< ThreadID > NewThread( ThreadStyle     threadStyle,
	                             ThreadEntryTPP  threadEntry,
	                             void*           threadParam,
	                             Size            stackSize,
	                             ThreadOptions   options,
	                             void**          threadResult )
	{
		Nucleus::OnlyOnce< RegisterThreadManagerErrors >();
		
		::ThreadID result;
		
		ThrowOSStatus( ::NewThread( threadStyle,
		                            threadEntry,
		                            threadParam,
		                            stackSize,
		                            options,
		                            threadResult,
		                            &result ) );
		
		return Nucleus::Owned< ThreadID >::Seize( ThreadID( result ) );
	}
	
	void SetThreadScheduler( ThreadSchedulerTPP threadScheduler )
	{
		Nucleus::OnlyOnce< RegisterThreadManagerErrors >();
		
		ThrowOSStatus( ::SetThreadScheduler( threadScheduler ) );
	}
	
	void SetThreadSwitcher( ThreadID         thread,
	                        ThreadSwitchTPP  threadSwitcher,
	                        void*            switchProcParam,
	                        bool             inVsOut )
	{
		Nucleus::OnlyOnce< RegisterThreadManagerErrors >();
		
		ThrowOSStatus( ::SetThreadSwitcher( thread, threadSwitcher, switchProcParam, inVsOut ) );
	}
	
	void SetThreadTerminator( ThreadID              thread,
	                          ThreadTerminationTPP  threadTerminator,
	                          void *                terminationProcParam )
	{
		Nucleus::OnlyOnce< RegisterThreadManagerErrors >();
		
		ThrowOSStatus( ::SetThreadTerminator( thread, threadTerminator, terminationProcParam ) );
	}
	
	void SetDebuggerNotificationProcs( DebuggerNewThreadTPP        notifyNewThread,
	                                   DebuggerDisposeThreadTPP    notifyDisposeThread,
	                                   DebuggerThreadSchedulerTPP  notifyThreadScheduler )
	{
		Nucleus::OnlyOnce< RegisterThreadManagerErrors >();
		
		ThrowOSStatus( ::SetDebuggerNotificationProcs( notifyNewThread,
		                                               notifyDisposeThread,
		                                               notifyThreadScheduler ) );
	}
	
	void CreateThreadPool( ThreadStyle  threadStyle,
	                       std::size_t  numToCreate,
	                       Size         stackSize )
	{
		Nucleus::OnlyOnce< RegisterThreadManagerErrors >();
		
		ThrowOSStatus( ::CreateThreadPool( threadStyle, numToCreate, stackSize ) );
	}
	
	std::size_t GetFreeThreadCount( ThreadStyle threadStyle )
	{
		Nucleus::OnlyOnce< RegisterThreadManagerErrors >();
		
		SInt16 result;
		ThrowOSStatus( ::GetFreeThreadCount( threadStyle, &result ) );
		
		return result;
	}
	
	std::size_t GetSpecificFreeThreadCount( ThreadStyle  threadStyle,
	                                        Size         stackSize )
	{
		Nucleus::OnlyOnce< RegisterThreadManagerErrors >();
		
		SInt16 result;
		ThrowOSStatus( ::GetSpecificFreeThreadCount( threadStyle, stackSize, &result ) );
		
		return result;
	}
	
	Size GetDefaultThreadStackSize( ThreadStyle threadStyle )
	{
		Nucleus::OnlyOnce< RegisterThreadManagerErrors >();
		
		::Size result;
		ThrowOSStatus( ::GetDefaultThreadStackSize( threadStyle, &result ) );
		
		return result;
	}
	
	std::size_t ThreadCurrentStackSpace( ThreadID thread )
	{
		Nucleus::OnlyOnce< RegisterThreadManagerErrors >();
		
		UInt32 result;
		ThrowOSStatus( ::ThreadCurrentStackSpace( thread, &result ) );
		
		return result;
	}
	
	void DisposeThread( Nucleus::Owned< ThreadID >  thread,
	                    void*              threadResult,
	                    bool               recycleThread )
	{
		Nucleus::OnlyOnce< RegisterThreadManagerErrors >();
		
		ThrowOSStatus( ::DisposeThread( thread.Release(), threadResult, recycleThread ) );
	}
	
	void YieldToThread( ThreadID suggestedThread )
	{
		Nucleus::OnlyOnce< RegisterThreadManagerErrors >();
		
		ThrowOSStatus( ::YieldToThread( suggestedThread ) );
	}
	
	void YieldToAnyThread()
	{
		Nucleus::OnlyOnce< RegisterThreadManagerErrors >();
		
		ThrowOSStatus( ::YieldToAnyThread() );
	}
	
	ThreadID MacGetCurrentThread()
	{
		Nucleus::OnlyOnce< RegisterThreadManagerErrors >();
		
		::ThreadID result;
		ThrowOSStatus( ::MacGetCurrentThread( &result ) );
		
		return result;
	}
	
	ThreadState GetThreadState( ThreadID thread )
	{
		Nucleus::OnlyOnce< RegisterThreadManagerErrors >();
		
		::ThreadState result;
		ThrowOSStatus( ::GetThreadState( thread, &result ) );
		
		return result;
	}
	
	void SetThreadState( ThreadID     threadToSet,
	                     ThreadState  newState,
	                     ThreadID     suggestedThread )
	{
		Nucleus::OnlyOnce< RegisterThreadManagerErrors >();
		
		ThrowOSStatus( ::SetThreadState( threadToSet,
		                                 newState,
		                                 suggestedThread ) );
	}
	
	void SetThreadStateEndCritical( ThreadID     threadToSet,
	                                ThreadState  newState,
	                                ThreadID     suggestedThread )
	{
		Nucleus::OnlyOnce< RegisterThreadManagerErrors >();
		
		ThrowOSStatus( ::SetThreadStateEndCritical( threadToSet,
		                                            newState,
		                                            suggestedThread ) );
	}
	
	void ThreadBeginCritical()
	{
		Nucleus::OnlyOnce< RegisterThreadManagerErrors >();
		
		ThrowOSStatus( ::ThreadBeginCritical() );
	}
	
	void ThreadEndCritical()
	{
		Nucleus::OnlyOnce< RegisterThreadManagerErrors >();
		
		ThrowOSStatus( ::ThreadEndCritical() );
	}
	
	ThreadTaskRef GetThreadCurrentTaskRef()
	{
		Nucleus::OnlyOnce< RegisterThreadManagerErrors >();
		
		::ThreadTaskRef result;
		ThrowOSStatus( ::GetThreadCurrentTaskRef( &result ) );
		
		return result;
	}
	
	ThreadState GetThreadStateGivenTaskRef( ThreadTaskRef  threadTaskRef,
	                                        ThreadID       threadToGet )
	{
		Nucleus::OnlyOnce< RegisterThreadManagerErrors >();
		
		::ThreadState result;
		ThrowOSStatus( ::GetThreadStateGivenTaskRef( threadTaskRef,
		                                             threadToGet,
		                                             &result ) );
		
		return result;
	}
	
	void SetThreadReadyGivenTaskRef( ThreadTaskRef  threadTaskRef,
	                                 ThreadID       threadToSet )
	{
		Nucleus::OnlyOnce< RegisterThreadManagerErrors >();
		
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

