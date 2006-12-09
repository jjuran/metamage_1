// Threads.cp

#ifndef NITROGEN_THREADS_H
#include "Nitrogen/Threads.h"
#endif

namespace Nitrogen
{
	
	ThreadManagerErrorsRegistrationDependency::ThreadManagerErrorsRegistrationDependency()
	{
		// does nothing, but guarantees construction of theRegistration
	}
	
	
	static void RegisterThreadManagerErrors();
	
	
	class ThreadManagerErrorsRegistration
	{
		public:
			ThreadManagerErrorsRegistration()  { RegisterThreadManagerErrors(); }
	};
	
	static ThreadManagerErrorsRegistration theRegistration;
	
	
	Nucleus::Owned< ThreadID > NewThread( ThreadStyle     threadStyle,
	                             ThreadEntryTPP  threadEntry,
	                             void*           threadParam,
	                             Size            stackSize,
	                             ThreadOptions   options,
	                             void**          threadResult )
	{
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
		ThrowOSStatus( ::SetThreadScheduler( threadScheduler ) );
	}
	
	void SetThreadSwitcher( ThreadID         thread,
	                        ThreadSwitchTPP  threadSwitcher,
	                        void*            switchProcParam,
	                        bool             inVsOut )
	{
		ThrowOSStatus( ::SetThreadSwitcher( thread, threadSwitcher, switchProcParam, inVsOut ) );
	}
	
	void SetThreadTerminator( ThreadID              thread,
	                          ThreadTerminationTPP  threadTerminator,
	                          void *                terminationProcParam )
	{
		ThrowOSStatus( ::SetThreadTerminator( thread, threadTerminator, terminationProcParam ) );
	}
	
	void SetDebuggerNotificationProcs( DebuggerNewThreadTPP        notifyNewThread,
	                                   DebuggerDisposeThreadTPP    notifyDisposeThread,
	                                   DebuggerThreadSchedulerTPP  notifyThreadScheduler )
	{
		ThrowOSStatus( ::SetDebuggerNotificationProcs( notifyNewThread,
		                                               notifyDisposeThread,
		                                               notifyThreadScheduler ) );
	}
	
	void CreateThreadPool( ThreadStyle  threadStyle,
	                       std::size_t  numToCreate,
	                       Size         stackSize )
	{
		ThrowOSStatus( ::CreateThreadPool( threadStyle, numToCreate, stackSize ) );
	}
	
	std::size_t GetFreeThreadCount( ThreadStyle threadStyle )
	{
		SInt16 result;
		ThrowOSStatus( ::GetFreeThreadCount( threadStyle, &result ) );
		
		return result;
	}
	
	std::size_t GetSpecificFreeThreadCount( ThreadStyle  threadStyle,
	                                        Size         stackSize )
	{
		SInt16 result;
		ThrowOSStatus( ::GetSpecificFreeThreadCount( threadStyle, stackSize, &result ) );
		
		return result;
	}
	
	Size GetDefaultThreadStackSize( ThreadStyle threadStyle )
	{
		::Size result;
		ThrowOSStatus( ::GetDefaultThreadStackSize( threadStyle, &result ) );
		
		return result;
	}
	
	std::size_t ThreadCurrentStackSpace( ThreadID thread )
	{
		UInt32 result;
		ThrowOSStatus( ::ThreadCurrentStackSpace( thread, &result ) );
		
		return result;
	}
	
	void DisposeThread( Nucleus::Owned< ThreadID >  thread,
	                    void*              threadResult,
	                    bool               recycleThread )
	{
		ThrowOSStatus( ::DisposeThread( thread.Release(), threadResult, recycleThread ) );
	}
	
	void YieldToThread( ThreadID suggestedThread )
	{
		ThrowOSStatus( ::YieldToThread( suggestedThread ) );
	}
	
	void YieldToAnyThread()
	{
		ThrowOSStatus( ::YieldToAnyThread() );
	}
	
	ThreadID MacGetCurrentThread()
	{
		::ThreadID result;
		ThrowOSStatus( ::MacGetCurrentThread( &result ) );
		
		return result;
	}
	
	ThreadState GetThreadState( ThreadID thread )
	{
		::ThreadState result;
		ThrowOSStatus( ::GetThreadState( thread, &result ) );
		
		return result;
	}
	
	void SetThreadState( ThreadID     threadToSet,
	                     ThreadState  newState,
	                     ThreadID     suggestedThread )
	{
		ThrowOSStatus( ::SetThreadState( threadToSet,
		                                 newState,
		                                 suggestedThread ) );
	}
	
	void SetThreadStateEndCritical( ThreadID     threadToSet,
	                                ThreadState  newState,
	                                ThreadID     suggestedThread )
	{
		ThrowOSStatus( ::SetThreadStateEndCritical( threadToSet,
		                                            newState,
		                                            suggestedThread ) );
	}
	
	void ThreadBeginCritical()
	{
		ThrowOSStatus( ::ThreadBeginCritical() );
	}
	
	void ThreadEndCritical()
	{
		ThrowOSStatus( ::ThreadEndCritical() );
	}
	
	ThreadTaskRef GetThreadCurrentTaskRef()
	{
		::ThreadTaskRef result;
		ThrowOSStatus( ::GetThreadCurrentTaskRef( &result ) );
		
		return result;
	}
	
	ThreadState GetThreadStateGivenTaskRef( ThreadTaskRef  threadTaskRef,
	                                        ThreadID       threadToGet )
	{
		::ThreadState result;
		ThrowOSStatus( ::GetThreadStateGivenTaskRef( threadTaskRef,
		                                             threadToGet,
		                                             &result ) );
		
		return result;
	}
	
	void SetThreadReadyGivenTaskRef( ThreadTaskRef  threadTaskRef,
	                                 ThreadID       threadToSet )
	{
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

